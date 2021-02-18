This is a stand-alone webapp that implements the user interface and business logic for the WebUSB programmer hardware.

Note that for testing you must serve the webapp from http://localhost/ as browsers do not allow WebUSB from HTTP, only secure locations like HTTPS (and HTTP on localhost).

## Build

    ./inline-media.py index.html >../../../../gh-pages/programmer.html
