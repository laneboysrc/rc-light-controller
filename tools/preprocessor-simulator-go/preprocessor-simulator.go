package main

import (
    "fmt"
    "log"
    "net/http"
    "strconv"
    "time"

    "github.com/google/gousb"
    "github.com/skratchdot/open-golang/open"
)

var receiver map[string]int

var ep_out *gousb.OutEndpoint
var ep_in *gousb.InEndpoint

const port = 8080

func httpHandler(w http.ResponseWriter, r *http.Request) {
    switch r.Method {
    case "GET":
        cookie := http.Cookie{Name: "mode", Value: "xhr", MaxAge: 1}
        http.SetCookie(w, &cookie)
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

        fmt.Fprintf(w, "OK")
        return

    default:
        http.Error(w, "Unsupported method", http.StatusMethodNotAllowed)
        return
    }
}

func writer() {
    for {
        time.Sleep(20 * time.Millisecond)

        data := make([]byte, 4)
        data[0] = byte(0x87)

        st := receiver["ST"]
        if st < 0 {
            st = 256 + st
        }

        th := receiver["TH"]
        if th < 0 {
            th = 256 + th
        }

        last_byte := 0
        if receiver["CH3"] != 0 {
            last_byte += 0x01
        }
        if receiver["STARTUP_MODE"] != 0 {
            last_byte += 0x10
        }

        data[1] = byte(st)
        data[2] = byte(th)
        data[3] = byte(last_byte)

        numBytes, err := ep_out.Write(data)
        if numBytes != 4 {
            log.Fatalf("%s.Write(): only %d bytes written, returned error is %v", ep_out, numBytes, err)
        }
    }
}

func reader() {
    buf := make([]byte, 64)

    for {
        time.Sleep(20 * time.Millisecond)

        numBytes, err:= ep_in.Read(buf)
        if err != nil {
            log.Fatalf("%s.Read(): returned error %v", ep_in, err)
        } else {
            log.Print(string(buf[:numBytes]))
        }
    }
}


func usbtest() {
    // Initialize a new Context.
    ctx := gousb.NewContext()
    // defer ctx.Close()

    // Open any device with a given VID/PID using a convenience function.
    dev, err := ctx.OpenDeviceWithVIDPID(0x6666, 0xcab1)
    if err != nil {
        log.Fatalf("Could not open a device: %v", err)
    }
    // defer dev.Close()

    cfg, err := dev.Config(1)
    if err != nil {
        log.Fatalf("%s.Config(1): %v", dev, err)
    }
    // defer cfg.Close()

    // In the config #1, claim interface #2 with alt setting #0.
    intf, err := cfg.Interface(1, 0)
    if err != nil {
        log.Fatalf("%s.Interface(1, 0): %v", cfg, err)
    }
    // defer intf.Close()

    // Open an OUT endpoint.
    ep_out, err = intf.OutEndpoint(2)
    if err != nil {
        log.Fatalf("%s.OutEndpoint(2): %v", intf, err)
    }

    // Open an IN endpoint.
    ep_in, err = intf.InEndpoint(1)
    if err != nil {
        log.Fatalf("%s.InEndpoint(1): %v", intf, err)
    }
}

func main() {
    receiver = make(map[string]int)

    usbtest()

    http.HandleFunc("/", httpHandler)

    go writer()
    go reader()

    url := fmt.Sprintf("http://localhost:%d/", port)

    open.Start(url)

    log.Printf("Please call up the user interface at %s", url)
    log.Fatal(http.ListenAndServe(fmt.Sprintf(":%d", port), nil))
}