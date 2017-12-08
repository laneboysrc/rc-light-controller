# Firmware
* All HAL functions start with uppercase HAL
* Use tiny printf instead of current uart0* functions
* Separate diagnostics from uart, so depending on HW we can do both in different configurations
* HAL should be called once every mainloop for service HAL function (USB!)
* Better soft timer based on ATMEL design idea?
* Steering wheel servo pulse is out of range after power on?

# PCB

* TLC5940 footprint based on TI datasheet
* Increase spacing of solder bridge to minimum allowed (6mil? 8mil?)
* Increase via size where possible
* Design for stencil 5mil/0.12mm thick