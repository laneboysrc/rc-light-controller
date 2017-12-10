# Firmware
* Use tiny printf instead of current uart0* functions
* Separate diagnostics from uart, so depending on HW we can do both in different configurations
    * How to do this with printf?
    * We make fprintf and fputc
* HAL should be called once every mainloop for service HAL function (USB!)
* Better soft timer based on ATMEL design idea?
* Steering wheel servo pulse is out of range after power on?
* Trigger bootloader from CDC like Arduino (1200 BAUD, DTR low)
    * Can we also detect BOSSAC?
* Monitor atsam91 erase command?
* servo_output_enable should only be activated when gearbox servo configured, to prevent glitch before initialized properly?

# PCB

* TLC5940 footprint based on TI datasheet
* Increase spacing of solder bridge to minimum allowed (6mil? 8mil?)
* Increase via size where possible
* Design for stencil 5mil/0.12mm thick