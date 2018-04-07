# Web-based light controller configuration tool

This tool compiles into a single HTML file that can be run locally by users to configure the light controller. The firmware image is already embedded in the tool, and the tool outputs a ``light_controller.hex`` file that can be directly flashed into the light controller with a simple USB-serial dongle.

This tool makes use of many **awesome** open source components. Refer to [configurator.html](src/index.html) for a full list.


## Build instructions

The "source" can be run and edited by simply opening [index.html](src/index.html) with the web browser.

To compile all source files into a single HTML file for distribution the following tools are needed:

- GNU Make - [https://www.gnu.org/software/make/](https://www.gnu.org/software/make/)
  Windows executable is available at [http://gnuwin32.sourceforge.net/packages/make.htm](http://gnuwin32.sourceforge.net/packages/make.htm)

- Node.js - [http://nodejs.org/](http://nodejs.org/)

After installing these tools, run ``make`` to generate the compiled HTML file in ``build/configurator.html``.

Note that the light program assembler needs to be compiled seperately if it has been modified. See instructions in the [assembler/](assembler/) folder.
