#!/usr/bin/env python
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
import os
import argparse
import serial
import time
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


class QuietBaseHTTPRequestHandler(BaseHTTPRequestHandler):
    def log_request(self, code, message=None):
        ''' Supress logging of HTTP requests '''
        pass


def parse_commandline():
    ''' Simulate a receiver with built-in preprocessor '''
    parser = argparse.ArgumentParser(
        description="Simulate a receiver with built-in preprocessor.")

    parser.add_argument("-b", "--baudrate", type=int, default=38400,
        help='Baudrate to use. Default is 38400.')

    parser.add_argument("-3", "--force-3ch", action='store_true',
        help='Force 3-channel preprocessor support')

    parser.add_argument("-5", "--force-5ch", action='store_true',
        help='Force 5-channel preprocessor support')

    parser.add_argument("-p", "--port", type=int, default=1234,
        help='HTTP port for the web UI. Default is localhost:1234.')

    parser.add_argument("-u", "--usb", "--webusb", action='store_true',
        help='Use WebUSB to connect to the light controller instead of a serial port')

    parser.add_argument("tty", nargs="?", default="/dev/ttyUSB0",
        help="Serial port to use. ")

    return parser.parse_args()


class CustomHTTPRequestHandler(QuietBaseHTTPRequestHandler):
    ''' Request handler that implements our simple web based API '''

    def do_GET(self):
        ''' GET request handler '''
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        if not self.server.preprocessor.args.usb:
            self.send_header('Set-Cookie', 'mode=xhr;max-age=1')
        self.end_headers()

        html_path = os.path.join(
            os.path.dirname(os.path.abspath(sys.argv[0])),
            HTML_FILE)

        with open(html_path, "r") as html_file:
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
        self.receiver = {
            'ST': 0, 'TH': 0, 'CH3': 0, 'AUX': 0, 'AUX2': 0, "AUX3": 0,
            'STARTUP_MODE': 1, 'PING' : 0}
        self.read_thread = None
        self.write_thread = None
        self.done = False
        self.config = ''

        if self.args.force_5ch and self.args.force_3ch:
            print("Error: 3 and 5 channel support can not be activated simultanously")
            sys.exit(1)

        if self.args.force_5ch:
            self.protocol = '5'
            self.args.multi_aux = True
            print("Manually setting 5-channel support")
        elif self.args.force_3ch:
            self.protocol = '3'
            self.args.multi_aux = False
            print("Manually setting 3-channel support")
        else:
            self.protocol = 'A'
            self.args.multi_aux = False

        if self.args.usb:
            return

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
        return 200, 'OK {} {}'.format(self.protocol, self.config)

    def reader(self):
        ''' Background thread performing the UART read '''
        time_of_last_line = start_time = time.time()
        print("     TOTAL  DIFFERENCE  RESPONSE")
        print("----------  ----------  --------")

        while not self.done:
            self.uart.timeout = 0.1
            try:
                data = self.uart.readline()
            except serial.SerialException as error:
                print("Reading from serial port failed: %s" % error)
                self.errorShutdown()
                return

            if data:
                message = data.decode('ascii', errors='replace')

                if message.startswith('CONFIG'):
                    self.config = message
                    if self.protocol == 'A':
                        self.args.multi_aux = (self.config.split(' ')[1] != '0')

                current_time = time.time()
                time_difference = current_time - time_of_last_line
                elapsed_time = current_time - start_time

                print("%10.3f  %10.3f  %s" % (elapsed_time,
                                              time_difference,
                                              message),
                      end='')

                time_of_last_line = current_time

    def writer(self):
        ''' Background thread performing the UART transmission '''
        while not self.done:
            steering = self.receiver['ST']
            if steering < 0:
                steering = 256 + steering

            throttle = self.receiver['TH']
            if throttle < 0:
                throttle = 256 + throttle

            mode_byte = 0
            if self.receiver['CH3']:
                mode_byte += 0x01
            if self.receiver['STARTUP_MODE']:
                mode_byte += 0x10
            if self.args.multi_aux:
                mode_byte += 0x08

            data = bytearray(
                [SLAVE_MAGIC_BYTE, steering, throttle, mode_byte])

            if self.args.multi_aux:
                aux = self.receiver['AUX']
                if aux < 0:
                    aux = 256 + aux

                aux2 = self.receiver['AUX2']
                if aux2 < 0:
                    aux2 = 256 + aux2

                aux3 = self.receiver['AUX3']
                if aux3 < 0:
                    aux3 = 256 + aux3

                data.extend([aux, aux2, aux3])

            try:
                self.uart.write(data)
                self.uart.flush()
            except serial.SerialException as error:
                print("Writing to serial port failed: %s" % error)
                self.errorShutdown()
                return

            time.sleep(0.02)

    def run(self):
        ''' Send the test patterns to the TLC5940 based slave '''
        self.server = HTTPServer(('', self.args.port), CustomHTTPRequestHandler)
        self.server.preprocessor = self

        print("Please call up the user interface on localhost:{port}".format(
            port=self.args.port))

        if not self.args.usb:
            self.read_thread = threading.Thread(target=self.reader, name='rx')
            self.read_thread.daemon = True
            self.read_thread.start()
            self.write_thread = threading.Thread(target=self.writer, name='tx')
            self.write_thread.daemon = True
            self.write_thread.start()
        self.server.serve_forever()

    def shutdown(self):
        ''' Shut down the application, wait for the uart thread to finish '''
        self.done = True
        if not self.write_thread is None:
            self.write_thread.join()
        if not self.read_thread is None:
            self.read_thread.join()

    def errorShutdown(self):
        ''' Shut down the application in case an error occured '''
        self.done = True
        self.uart.close()
        self.server.shutdown()
        sys.exit(1)

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
