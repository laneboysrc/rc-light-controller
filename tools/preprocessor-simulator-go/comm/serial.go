package comm

import (
	"fmt"
	"time"

	"github.com/tarm/serial"
)

func serialControl(port string, baudrate int) {
	first := true

	for {
		func() {
			c := &serial.Config{Name: port, Baud: baudrate}
			s, err := serial.OpenPort(c)
			if err != nil {
				if first {
					first = false
					disconnected(fmt.Sprintf("Can not open %s: %s", port, err))
				}
				return
			}
			defer s.Close()
			first = true

			connected(fmt.Sprintf("Connected on serial port %s at %d BAUD", port, baudrate))
			communicate(s, s)
			disconnected("Serial port connection closed")
		}()

		time.Sleep(retryTimeout)
	}
}