/*

Simulate a receiver with built-in preprocessor. This allow testing of the
light controller functionality without hooking up a RC system.

A web browser is used for the user interface.

*/
package main

import (
	"flag"
	"fmt"
	"log"

	"github.com/skratchdot/open-golang/open"

	"github.com/laneboysrc/rc-light-controller/tools/preprocessor-simulator-go/comm"
	"github.com/laneboysrc/rc-light-controller/tools/preprocessor-simulator-go/server"
)

// Command line parameters
var (
	useWebusb     = flag.Bool("webusb", false, "use WebUSB to connect to the light controller")
	port          = flag.Int("p", 1234, "HTTP `port` for the web UI")
	baudrate      = flag.Int("b", 38400, "`baudrate` to use. Only applicable when -tty is specified")
	serialPort    = flag.String("tty", "", "serial port connection to the light controller. If not specified, USB is used")
	launchBrowser = flag.Bool("l", false, "launch the app in the web browser")
)

func main() {
	flag.Parse()

	switch {
	default:
		go comm.UseUsb()
	case *serialPort != "":
		go comm.UseSerial(*serialPort, *baudrate)
	case *useWebusb:
		// Do nothing, everything is handled by the HTML
	}

	url := fmt.Sprintf("http://localhost:%d/", *port)

	if *launchBrowser {
		open.Start(url)
	}

	log.Printf("Please call up the user interface at %s", url)
	server.RegisterHttp(*useWebusb)
	server.RegisterWebsocket()
	log.Fatal(server.Run(fmt.Sprintf(":%d", *port)))
}
