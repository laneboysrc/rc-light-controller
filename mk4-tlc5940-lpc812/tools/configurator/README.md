# Web-based light controller configuration tool

This tool compiles into a single HTML file that can be run locally by users to configure the light controller. The firmware image is already embedded in the tool, and the tool outputs a ``light_controller.hex`` file that can be directly flashed into the light controller with a simple USB-serial dongle.

This tool makes use of many **awesome** open source components. Refer to [configurator.html](configurator.html) for a full list.


## Build instructions

The "source" can be run and edited by simply opening [configurator.html](configurator.html) with the web browser.

To compile all source files into a single HTML file for distribution the following tools are needed:

- Node.js - [http://nodejs.org/](http://nodejs.org/)

- gulp.js - [http://gulpjs.com/](http://gulpjs.com/)

After installing these tools, run ``npm install`` to fetch the required nodejs and gulp modules.

Then run ``gulp``, which will generate the compiled HTML file in ``build/configurator.html``.

Note that the light program assembler needs to be compiled seperately if it has been modified. See instructions in the [assembler/](assembler/) folder.
