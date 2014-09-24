'''
preprocessor-simulator.py

Simulate a receiver with built-in preprocessor. This allow testing of the
light controller functionality without hooking up a RC system.

A web browser is used for the user interface

Author:         Werner Lane
E-mail:         laneboysrc@gmail.com
'''

from __future__ import print_function

import sys
import argparse
import serial
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
from urlparse import urlparse, parse_qs

HTML = '''\
<!doctype html>
<html class="no-js" lang="en">
    <head>
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <title>Preprocessor simulator - DIY RC Light Controller</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body>
        <h1>Preprocessor simulator</h1>
        <h2>DIY RC Light Controller</h2>
        <div >
            <input type="range" min="-100" max="100" step="1" name="st"
                value="0" style="width:80%;" onchange="send('st', this.value);",
                oninput="send('st', this.value);" />
        </div>
        <div>
            Throttle
        </div>
        <div>
            CH3 (AUX)
        </div>

        <div id="response">
        </div>

        <script>
            function createXMLHttpRequest() {
                try { return new XMLHttpRequest(); } catch(e) {}
                try { return new ActiveXObject("Msxml2.XMLHTTP"); } catch (e) {}
                return null;
            }

            function send(type, value) {
                var params = type + "=" + value;

                var xhr = createXMLHttpRequest();
                xhr.onerror = function(evt) {
                    msg = "ERR Unable to send XMLHttpRequest";
                    document.getElementById("response").innerHTML = msg;
                }

                xhr.onload = function(evt) {
                    document.getElementById("response").innerHTML = xhr.responseText;
                }

                xhr.open("GET", "/api?" + params, true);

                //xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
                xhr.setRequestHeader("Connection", "close");

                xhr.send(params);
            }
        </script>
    </body>
</html>
'''


def parse_commandline():
    ''' Simulate a receiver with built-in preprocessor '''
    parser = argparse.ArgumentParser(
        description="Simulate a receiver with built-in preprocessor.")

    parser.add_argument("-b", "--baudrate", type=int, default=38400,
        help='Baudrate to use. Default is 38400.')

    parser.add_argument("-p", "--port", type=int, default=1234,
        help='HTTP port for the web UI. Default is localhost:1234.')

    parser.add_argument("tty", nargs="?", default="/dev/ttyUSB0",
        help="serial port to use. ")

    return parser.parse_args()


class CustomHTTPRequestHandler(BaseHTTPRequestHandler):
    ''' Request handler that implements our simple web based API '''

    def do_GET(self):
        ''' GET request handler '''
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(HTML)

        elif self.path.startswith('/api?'):
            try:
                query = parse_qs(urlparse(self.path).query, strict_parsing=True)

            except ValueError:
                self.send_response(400)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
                self.wfile.write("Bad querystring")

            else:
                response, content = self.server.preprocessor.api(query)
                self.send_response(response)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
                self.wfile.write(content)

        else:
            self.send_response(404)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write("Not found")
        return

    def do_POST(self):
        ''' POST request handler '''
        self.send_response(405)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write("POST not implemented")
        return


class PreprocessorApp(object):
    ''' Simulate a RC receiver with preprocessor '''

    def __init__(self):
        self.args = parse_commandline()

        try:
            self.uart = serial.Serial(self.args.tty, self.args.baudrate)
        except serial.SerialException as error:
            print("Unable to open port %s: %s" % (self.args.tty, error))
            sys.exit(1)

        print("Simulating on {uart} at {baudrate} baud.".format(
            uart=self.uart.port, baudrate=self.uart.baudrate))

    def api(self, query):
        ''' Web api handler '''
        print(query)
        return 200, "OK"

    def run(self):
        ''' Send the test patterns to the TLC5940 based slave '''
        server = HTTPServer(('', self.args.port), CustomHTTPRequestHandler)
        server.preprocessor = self
        print("Please call up the user interface on localhost:{port}".format(
            port=self.args.port))
        server.serve_forever()


if __name__ == '__main__':
    try:
        PreprocessorApp().run()
    except KeyboardInterrupt:
        print("")
        sys.exit(0)

