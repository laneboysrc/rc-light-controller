package main

import (
    "fmt"
    "log"
    "net/http"
    "flag"
    "strconv"
    "time"
    "io"

    "github.com/google/gousb"
    // "github.com/skratchdot/open-golang/open"
)

var receiver map[string]int = make(map[string]int)

var ep_out *gousb.OutEndpoint
var ep_in *gousb.InEndpoint

var useWebusb bool

func httpHandler(w http.ResponseWriter, r *http.Request) {
    switch r.Method {
    case "GET":
        if !useWebusb {
            cookie := http.Cookie{Name: "mode", Value: "xhr", MaxAge: 1}
            http.SetCookie(w, &cookie)
        }
        http.ServeFile(w, r, "./preprocessor-simulator.html")
        return

    case "POST":
        err := r.ParseForm()
        if err != nil {
            log.Println(err)
            http.Error(w, "Form parsing failed", http.StatusInternalServerError)
            return
        }

        for name, values := range(r.Form) {
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


func writer(port io.Writer) {
    const SLAVE_MAGIC_BYTE = 0x87

    for {
        last_byte := 0
        if receiver["CH3"] != 0 {
            last_byte += 0x01
        }
        if receiver["STARTUP_MODE"] != 0 {
            last_byte += 0x10
        }

        data := make([]byte, 4)
        data[0] = byte(SLAVE_MAGIC_BYTE)
        data[1] = uint8(receiver["ST"])
        data[2] = uint8(receiver["TH"])
        data[3] = byte(last_byte)

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
        numBytes, err:= port.Read(buf)
        if err != nil {
            log.Printf("%s.Read(): returned error %v", port, err)
            return
        } else {
            log.Print(string(buf[:numBytes]))
        }
    }
}

func serialControl(serialPort string) {
    log.Printf("serialControl is not implemented yet. tty=%s", serialPort)
}

func usbControl() {
    first := true

    for {
        func () {
            ctx := gousb.NewContext()
            defer ctx.Close()

            dev, err := ctx.OpenDeviceWithVIDPID(0x6666, 0xcab1)
            if err != nil {
                log.Fatalf("Could not open a device: %v", err)
            }
            if dev == nil {
                if first {
                    first = false
                    log.Printf("No LANE Boys RC Light Controller connected via USB")
                }
                return;
            }
            defer dev.Close()
            first = false

            cfg, err := dev.Config(1)
            if err != nil {
                log.Printf("%s.Config(1): %v", dev, err)
                return;
            }
            defer cfg.Close()

            intf, err := cfg.Interface(1, 0)
            if err != nil {
                log.Printf("%s.Interface(1, 0): %v", cfg, err)
                return;
            }
            defer intf.Close()

            ep_out, err = intf.OutEndpoint(2)
            if err != nil {
                log.Printf("%s.OutEndpoint(2): %v", intf, err)
                return
            }

            ep_in, err = intf.InEndpoint(1)
            if err != nil {
                log.Printf("%s.InEndpoint(1): %v", intf, err)
                return
            }

            if serial, err := dev.SerialNumber(); err == nil {
                log.Printf("Connected to Light Controller with serial number %s", serial)
            }

            receiver["CONNECTED"] = 1
            go writer(ep_out)
            reader(ep_in)
            log.Printf("Device closed, scanning for new one")
        }()

        receiver["CONNECTED"] = 0
        time.Sleep(100 * time.Millisecond)
    }
}

func main() {
    var port int
    var baudrate int
    var serialPort string

    flag.IntVar(&baudrate, "baudrate", 38400, "Baudrate to use. Default is 38400.")
    flag.IntVar(&baudrate, "b", 38400, "Baudrate to use. Default is 38400.")
    flag.IntVar(&port, "port", 1234, "HTTP port for the web UI. Default is localhost:1234.")
    flag.IntVar(&port, "p", 1234, "HTTP port for the web UI. Default is localhost:1234.")
    flag.StringVar(&serialPort, "tty", "", "Serial port to use. If not specified, USB is used.")
    flag.BoolVar(&useWebusb, "webusb", false, "Use WebUSB to connect to the light controller")
    flag.BoolVar(&useWebusb, "u", false, "Use WebUSB to connect to the light controller")
    flag.Parse()

    if useWebusb {
        // Do nothing, everything is handled by the HTML
    } else if serialPort != "" {
        serialControl(serialPort)
    } else {
        go usbControl()
    }

    http.HandleFunc("/", httpHandler)
    url := fmt.Sprintf("http://localhost:%d/", port)
    // open.Start(url)
    log.Printf("Please call up the user interface at %s", url)
    log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}