package comm

import (
	"io"
	"log"
	"strings"
	"sync"
	"time"

	"github.com/laneboysrc/rc-light-controller/tools/preprocessor-simulator-go/receiver"
	"github.com/laneboysrc/rc-light-controller/tools/preprocessor-simulator-go/server"
)

var retryTimeout = 500 * time.Millisecond

func UseUsb() {
	usbControl()
}

func UseSerial(port string, baudrate int) {
	serialControl(port, baudrate)
}

func connected(msg string) {
	receiver.Set("CONNECTED", 1)
	server.Publish(msg)
}

func disconnected(msg string) {
	receiver.Set("CONNECTED", 0)
	server.Publish(msg)
}

func communicate(r io.Reader, w io.Writer) {
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

func writer(port io.Writer) {
	const slaveMagicByte = 0x87

	for {
		data := make([]byte, 4)
		lastByte := 0

		if receiver.Get("CH3") != 0 {
			lastByte += 0x01
		}
		if receiver.Get("STARTUP_MODE") != 0 {
			lastByte += 0x10
		}

		data[0] = byte(slaveMagicByte)
		data[1] = uint8(receiver.Get("ST"))
		data[2] = uint8(receiver.Get("TH"))
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

		// Print the message on the console and in the web US. Note that the
		// message could have newlines in it, we parse that out.
		arr := strings.Split(strings.Trim(message, "\n"), "\n")
		for _, msg := range arr {
			server.Publish(msg)
		}
	}
}
