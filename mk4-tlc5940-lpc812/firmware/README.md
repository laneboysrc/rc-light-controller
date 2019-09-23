# Build instructions for the MK4 TLC5940 LPC812 variant

Note: most users will not need to build the firmware as it can be fully configured through a web-based configuration tool!


## Tools required:

- GCC for ARM - [https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)

- GNU Make - [https://www.gnu.org/software/make/](https://www.gnu.org/software/make/)
  Windows executable is available at [http://gnuwin32.sourceforge.net/packages/make.htm](http://gnuwin32.sourceforge.net/packages/make.htm)

- Windows users: **rm.exe** from [coreutils](http://gnuwin32.sourceforge.net/downlinks/coreutils-bin-zip.php).


# Building the firmware

Running ``make`` in a console window in this directory builds the firmware. The resulting *firmware.bin* and *firmware.hex* are located in the build directory.

Running ``make program`` flashes the firmware, assuming you are using the [LCP81x-ISP](https://github.com/laneboysrc/LPC81x-ISP-tool) tool.


Files in the firmware root directory are source code that is processor hardware independent. The *hal/* folder contains processor-specific light controller functionality such as MCU initialization, GPIO, SPI, UART etc.
*LPC8xx/* contains header files provided by NXP for the LPC812 MCU. *light_programs/* contains example light programs you can use for reference.
Output files of the build process are placed in the *build/* folder.