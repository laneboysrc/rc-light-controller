This tool simulates a receiver with a S-Bus output, using a USB-to-Serial adapter.

*Important*: you need a USB-to-Serial adapter that is capable of using 100000 BAUD. Many commercially available adapters do not support this non-standard Baudrate.

Note that for testing you must serve the webapp from http://localhost/ as browsers do not allow WebUSB from HTTP, only secure locations like HTTPS (and HTTP on localhost).

## Build instructions

    cd webapp
    python3 inline-media.py index.html >../../../../gh-pages/sbus-webserial.html

