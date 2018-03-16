package comm

import (
	"fmt"
	"log"
	"time"

	"github.com/google/gousb"
)

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
					disconnected(fmt.Sprintf("No LANE Boys RC Light Controller connected via USB"))
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
				connected(fmt.Sprintf("Connected to Light Controller with serial number %s", serial))
			} else {
				connected("Connected to Light Controller via USB")
			}
			communicate(epIn, epOut)
			disconnected("USB connection closed")
		}()

		time.Sleep(retryTimeout)
	}
}