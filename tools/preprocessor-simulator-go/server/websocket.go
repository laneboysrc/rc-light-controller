package server

import (
	"encoding/json"
	"log"
	"net/http"
	"sync"

	"github.com/gorilla/websocket"
	"github.com/laneboysrc/rc-light-controller/tools/preprocessor-simulator-go/receiver"
)

type session struct {
	id int
	c  *websocket.Conn
}

var sessions = make(map[int]*session)
var sessionsMutex sync.Mutex
var latestSessionId = 0

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

func RegisterWebsocket() {
	http.HandleFunc("/websocket", websocketHandler)
}

func Publish(message string) {
	log.Printf(message)

	sessionsMutex.Lock()
	for _, s := range sessions {
		s.c.WriteMessage(websocket.TextMessage, []byte(message))
	}
	sessionsMutex.Unlock()
}

func newSession() int {
	latestSessionId++
	return latestSessionId
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
						receiver.Set(n, int(v))
					}
				}
			}
		}
	}

	sessionsMutex.Lock()
	delete(sessions, s.id)
	sessionsMutex.Unlock()
}
