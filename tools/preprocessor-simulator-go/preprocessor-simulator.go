/*

Simulate a receiver with built-in preprocessor. This allow testing of the
light controller functionality without hooking up a RC system.

A web browser is used for the user interface.

*/
package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"strconv"
	"sync"
	"time"

	"github.com/google/gousb"
	"github.com/skratchdot/open-golang/open"
	"github.com/tarm/serial"
	"github.com/gorilla/websocket"
)

var receiver = make(map[string]int)

// Command line parameters
var (
	useWebusb     bool
	port          int
	baudrate      int
	serialPort    string
	launchBrowser bool
)

var retryTimeout = 500 * time.Millisecond

var upgrader = websocket.Upgrader{
    ReadBufferSize:  1024,
    WriteBufferSize: 1024,
}

type session struct {
	id 	int
	c 	*websocket.Conn
}

var sessions = make(map[int]*session)
var sessionsMutex sync.Mutex
var latestSessionId = 0

func newSession() int {
	latestSessionId++
	return latestSessionId
}

func httpHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case "GET":
		if !useWebusb {
			// cookie := http.Cookie{Name: "mode", Value: "xhr", MaxAge: 1}
			cookie := http.Cookie{Name: "mode", Value: "ws", MaxAge: 1}
			http.SetCookie(w, &cookie)
		}
		http.ServeFile(w, r, "../preprocessor-simulator.html")
		return

	case "POST":
		err := r.ParseForm()
		if err != nil {
			log.Println(err)
			http.Error(w, "Form parsing failed", http.StatusInternalServerError)
			return
		}

		for name, values := range r.Form {
			if value, err := strconv.ParseInt(values[0], 10, 32); err == nil {
				receiver[name] = int(value)
			}
		}

		if receiver["CONNECTED"] != 0 {
			fmt.Fprintf(w, "OK")
		} else {
			fmt.Fprintf(w, "No device connected")
		}
		return

	default:
		http.Error(w, "Unsupported method", http.StatusMethodNotAllowed)
		return
	}
}

func websocketHandler(w http.ResponseWriter, r *http.Request) {
    conn, err := upgrader.Upgrade(w, r, nil)
    if err != nil {
        log.Println(err)
        return
    }
    log.Printf("Websocket connection established")

    go websocketLoop(conn)
}

func websocketLoop(c *websocket.Conn) {
	sessionsMutex.Lock()
	s := &session{id: newSession(), c: c}
	sessions[s.id] = s
	sessionsMutex.Unlock()

    for {
    	messageType, r, err := c.NextReader()
        if err != nil {
        	log.Printf("Websocket: closed")
            c.Close()
            break
        }

        if messageType == websocket.TextMessage {
			var m map[string]interface{}

			dec := json.NewDecoder(r)
			if err := dec.Decode(&m); err != nil {
            	log.Println(err)
            	continue
        	}

        	for n, d := range m {
        		switch d.(type) {
        		case float64:
        			v, ok := d.(float64)
        			if ok {
        				receiver[n] = int(v)
        			}
        		}
			}
        }
    }

	sessionsMutex.Lock()
    delete(sessions, s.id)
	sessionsMutex.Unlock()
}

func writer(port io.Writer) {
	const slaveMagicByte = 0x87

	for {
		lastByte := 0
		if receiver["CH3"] != 0 {
			lastByte += 0x01
		}
		if receiver["STARTUP_MODE"] != 0 {
			lastByte += 0x10
		}

		data := make([]byte, 4)
		data[0] = byte(slaveMagicByte)
		data[1] = uint8(receiver["ST"])
		data[2] = uint8(receiver["TH"])
		data[3] = byte(lastByte)

		numBytes, err := port.Write(data)
		if numBytes != 4 {
			log.Printf("%s.Write(): only %d bytes written, returned error is %v", port, numBytes, err)
			return
		}

		time.Sleep(20 * time.Millisecond)
	}
}

func reader(port io.Reader) {
	buf := make([]byte, 64)

	for {
		numBytes, err := port.Read(buf)
		if err != nil {
			log.Printf("%s.Read(): returned error %v", port, err)
			return
		}

		message := string(buf[:numBytes])

		// Print the message on the console
		log.Print(message)

		// Also send the message to all websocket clients
		sessionsMutex.Lock()
		for _, s := range sessions {
			s.c.WriteMessage(websocket.TextMessage, []byte(message))
		}
		sessionsMutex.Unlock()
	}
}

func useDevice(w io.Writer, r io.Reader) {
	// We use sync.WaitGroup to wait for the two goroutines reader() and
	// writer() to finish. To do so we Add(2), and defer Done() in each
	// goroutine. We the use Wait() to wait for the goroutines to
	// finish.
	var wg sync.WaitGroup

	wg.Add(2)
	go func() {
		defer wg.Done()
		writer(w)
	}()

	go func() {
		defer wg.Done()
		reader(r)
	}()

	wg.Wait()
}

func serialControl(serialPort string, baudrate int) {
	first := true

	for {
		func() {
			c := &serial.Config{Name: serialPort, Baud: baudrate}
			s, err := serial.OpenPort(c)
			if err != nil {
				if first {
					first = false
					log.Printf("Can not open %s: %s", serialPort, err)
				}
				return
			}
			defer s.Close()

			receiver["CONNECTED"] = 1
			log.Printf("Connected on serial port %s at %d BAUD", serialPort, baudrate)
			useDevice(s, s)
			log.Printf("Serial port connection closed")
		}()

		receiver["CONNECTED"] = 0
		time.Sleep(retryTimeout)
	}
}

func usbControl() {
	first := true

	for {
		func() {
			context := gousb.NewContext()
			defer context.Close()

			dev, err := context.OpenDeviceWithVIDPID(0x6666, 0xcab1)
			if err != nil {
				log.Fatalf("Could not open a device: %v", err)
			}
			if dev == nil {
				if first {
					first = false
					log.Printf("No LANE Boys RC Light Controller connected via USB")
				}
				return
			}
			defer dev.Close()
			first = false

			cfg, err := dev.Config(1)
			if err != nil {
				log.Printf("%s.Config(1): %v", dev, err)
				return
			}
			defer cfg.Close()

			intf, err := cfg.Interface(1, 0)
			if err != nil {
				log.Printf("%s.Interface(1, 0): %v", cfg, err)
				return
			}
			defer intf.Close()

			epOut, err := intf.OutEndpoint(2)
			if err != nil {
				log.Printf("%s.OutEndpoint(2): %v", intf, err)
				return
			}

			epIn, err := intf.InEndpoint(1)
			if err != nil {
				log.Printf("%s.InEndpoint(1): %v", intf, err)
				return
			}

			if serial, err := dev.SerialNumber(); err == nil {
				log.Printf("Connected to Light Controller with serial number %s", serial)
			}

			receiver["CONNECTED"] = 1
			useDevice(epOut, epIn)
			log.Printf("USB connection closed")
		}()

		receiver["CONNECTED"] = 0
		time.Sleep(retryTimeout)
	}
}

func init() {
	flag.IntVar(&port, "p", 1234, "HTTP `port` for the web UI")
	flag.StringVar(&serialPort, "tty", "", "serial port connection to the light controller. If not specified, USB is used")
	flag.IntVar(&baudrate, "b", 38400, "`baudrate` to use. Only applicable when -tty is specified")
	flag.BoolVar(&useWebusb, "webusb", false, "use WebUSB to connect to the light controller")
	flag.BoolVar(&launchBrowser, "l", false, "launch the app in the web browser")
}

func main() {
	flag.Parse()

	switch {
	default:
		go usbControl()
	case serialPort != "":
		go serialControl(serialPort, baudrate)
	case useWebusb:
		// Do nothing, everything is handled by the HTML
	}

	url := fmt.Sprintf("http://localhost:%d/", port)

	if launchBrowser {
		open.Start(url)
	}

	log.Printf("Please call up the user interface at %s", url)
	http.HandleFunc("/", httpHandler)
	http.HandleFunc("/websocket", websocketHandler)
	log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}
