all: run

ASSET_PATH = ..

server/bindata.go: $(ASSET_PATH)/preprocessor-simulator.html
	go-bindata -nometadata -pkg server -o $@ -prefix $(ASSET_PATH) $^

run: server/bindata.go
	go run main.go

build: server/bindata.go
	go build

clean:
	$(RM) -f preprocessor-simulator-go server/bindata.go

.PHONY: run build clean
