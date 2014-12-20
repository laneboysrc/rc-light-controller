# Build instructions for the MK4 TLC5940 LPC812 variant

Note: most users will not need to build the firmware as it can be fully configured through a web-based configuration tool!


## Tools required:

- GCC for ARM - [https://launchpad.net/gcc-arm-embedded/](https://launchpad.net/gcc-arm-embedded/)

- GNU Make - [https://www.gnu.org/software/make/](https://www.gnu.org/software/make/)
  Windows executable is available at [http://gnuwin32.sourceforge.net/packages/make.htm](http://gnuwin32.sourceforge.net/packages/make.htm)

- Windows users: **rm.exe** from [coreutils](http://gnuwin32.sourceforge.net/downlinks/coreutils-bin-zip.php).


# Building the firmware

Running ``make`` in a console window in this directory builds the firmware. The resulting *firmware.bin* and *firmware.hex* are located in the build directory.

Running ``make program`` flashes the firmware, assuming you are using the [LCP81x-ISP](https://github.com/laneboysrc/LPC81x-ISP-tool) tool.
