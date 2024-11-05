# ch55x-webusb-flasher

This is a web-based application that allows flashing of a CH552 microcontroller. It works with the standard v2.x bootloader found in CH552, but also has an additional feature to reboot the LANE Boys RC WebUSB programmer into the bootloader automaticlly.

This folder further contains documentation of the CH55x bootloader, which we made from bootloader source code dumps found on the Internet because we could not find concise documentation of the protocol.

This programmer only works with Web browsers that support WebUSB, such as Google Chrome and Microsoft Edge. https://caniuse.com/webusb

Because WebUSB is a protected function, you need to run index.html using https://. It is also possible to run it by accessing http://localhost/ (require running a development server on the local computer).
