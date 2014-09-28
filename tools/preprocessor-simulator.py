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
from time import sleep
import threading

try:
    from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
except ImportError:
    from http.server import BaseHTTPRequestHandler, HTTPServer

try:
    from urlparse import parse_qs
except ImportError:
    from urllib.parse import parse_qs


SLAVE_MAGIC_BYTE = 0x87
HTML_FILE = "preprocessor-simulator.html"


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
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        with open(HTML_FILE, "r") as html_file:
            self.wfile.write(html_file.read().encode("UTF-8"))
        return

    def do_POST(self):
        ''' POST request handler '''
        query = self.rfile.read(
            int(self.headers['Content-Length'])).decode("UTF-8")
        try:
            query = parse_qs(query, strict_parsing=True)

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
            self.wfile.write(content.encode('UTF-8'))
        return


class PreprocessorApp(object):
    ''' Simulate a RC receiver with preprocessor '''

    def __init__(self):
        self.args = parse_commandline()
        self.receiver = {'ST': 0, 'TH': 0, 'CH3': 0, 'STARTUP_MODE': 1}
        self.uart_thread = None
        self.done = False

        try:
            self.uart = serial.Serial(self.args.tty, self.args.baudrate)
        except serial.SerialException as error:
            print("Unable to open port %s: %s" % (self.args.tty, error))
            sys.exit(1)

        print("Simulating on {uart} at {baudrate} baud.".format(
            uart=self.uart.port, baudrate=self.uart.baudrate))

    def api(self, query):
        ''' Web api handler '''
        for key, value in query.items():
            if key in self.receiver:
                self.receiver[key] = int(value[0])
            else:
                return 400, "Bad request"
        return 200, "OK"

    def run(self):
        ''' Send the test patterns to the TLC5940 based slave '''
        server = HTTPServer(('', self.args.port), CustomHTTPRequestHandler)
        server.preprocessor = self

        def sender(app):
            ''' Background thread performing the UART transmission '''
            while not app.done:
                steering = app.receiver['ST']
                if steering < 0:
                    steering = 256 + steering

                throttle = app.receiver['TH']
                if throttle < 0:
                    throttle = 256 + throttle

                last_byte = 0
                if app.receiver['CH3']:
                    last_byte += 0x01
                if app.receiver['STARTUP_MODE']:
                    last_byte += 0x10

                data = bytearray(
                    [SLAVE_MAGIC_BYTE, steering, throttle, last_byte])
                app.uart.write(data)
                app.uart.flush()
                sleep(0.02)

        print("Please call up the user interface on localhost:{port}".format(
            port=self.args.port))

        self.uart_thread = threading.Thread(target=sender, args=([self]))
        self.uart_thread.start()
        server.serve_forever()

    def shutdown(self):
        ''' Shut down the application, wait for the uart thread to finish '''
        self.done = True
        if not self.uart_thread is None:
            self.uart_thread.join()


def main():
    ''' Program start '''
    app = PreprocessorApp()
    try:
        app.run()
    except KeyboardInterrupt:
        print("")
        app.shutdown()
        sys.exit(0)


if __name__ == '__main__':
    main()
