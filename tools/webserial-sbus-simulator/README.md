This tool simulates a receiver with a S.Bus output, using a USB-to-Serial adapter.

*Important*: you need a USB-to-Serial adapter that is capable of using 100000 BAUD. Many commercially available adapters do **not** support this **non-standard Baudrate**.

*Important*: The **TX line** of the USB-to-Serial adapter **needs to be inverted** before fed into the **ST/Rx pin** on the Light Controller. You need dedicated hardware to do this (e.g. use a 74HCU04 hex inverter).


## Build instructions for deployment on github.io

    cd webapp
    python3 inline-media.py index.html >../../../../gh-pages/sbus-webserial.html

