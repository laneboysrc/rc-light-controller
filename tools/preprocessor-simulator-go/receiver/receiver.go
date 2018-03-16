package receiver

import (
	"sync"
)

var receiver = make(map[string]int)
var m sync.Mutex

func Set(k string, v int) {
	m.Lock()
	receiver[k] = v
	m.Unlock()
}

func Get(k string) int {
	m.Lock()
	v := receiver[k]
	m.Unlock()

	return v
}
