# Light controller MK4 TLC5940 LPC812

For configuring and building the firmware, please download **[mk4-download-me.zip](mk4-download-me.zip)**.

This archive contains the configuration tool *configurator.html*. Open this file in your web browser (tested with Firefox, Chrome and Internet Explorer 10).
**configurator.html has the firmware already embedded**, just set the options you want and click on the *Save firmware image...* button.

The configured firmware image *light-controller.hex* will be stored in the *Download* folder of your web browser.

Use the LPC81x-ISP tool included in the archive to flash the firmware. You need a USB-to-serial adapter to connect your PC to the light controller.

Wire-up the USB-to-serial adapter as follows:

![Connecting the USB-to-serial adapter](doc/mk4-tlc5940-lpc812-programming.jpg)


In the rare event you want to modify the firmware, please consult [README.md](firmware/README.md) for build instructions.
