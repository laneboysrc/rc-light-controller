# dfu-detach

This tool allows to reboot the CH552-based WebUSB programmer rev 3 to reboot
into the CH552 bootloader form the command line.

This way we can automatically flash a new firmware using the chprog tool from
within the Makefile.

ole00/chprog: Yet another CH55x programmer with v1 and v2 bootloader detection and support.
https://github.com/ole00/chprog

This tool is designed to be compiled and run on Linux, or any other operating system that has gcc and libusb v1.
