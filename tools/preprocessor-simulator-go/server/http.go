package server

import (
	"fmt"
	"log"
	"net/http"
	"strconv"

	"github.com/laneboysrc/rc-light-controller/tools/preprocessor-simulator-go/receiver"
)

var setModeCookie bool

func RegisterHttp(useWebusb bool) {
	setModeCookie = !useWebusb
	http.HandleFunc("/", httpHandler)
}

func Run(h string) error {
	return http.ListenAndServe(h, nil)
}

func httpHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case "GET":
		if setModeCookie {
			cookie := http.Cookie{Name: "mode", Value: "ws", MaxAge: 2}
			http.SetCookie(w, &cookie)
		}

		data, err := Asset("preprocessor-simulator.html")
		if err != nil {
			http.NotFound(w, r)
		} else {
			w.Header().Set("Content-Type", "text/html; charset=utf-8")
			w.WriteHeader(http.StatusOK)
			fmt.Fprint(w, string(data))
		}

	case "POST":
		err := r.ParseForm()
		if err != nil {
			log.Println(err)
			http.Error(w, "Form parsing failed", http.StatusInternalServerError)
			return
		}

		for name, values := range r.Form {
			if value, err := strconv.ParseInt(values[0], 10, 32); err == nil {
				receiver.Set(name, int(value))
			}
		}

		if receiver.Get("CONNECTED") != 0 {
			fmt.Fprintf(w, "OK")
		} else {
			fmt.Fprintf(w, "No device connected")
		}

	default:
		http.Error(w, "Unsupported method", http.StatusMethodNotAllowed)
	}
}
