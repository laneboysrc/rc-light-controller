# Firmware
* Use tiny printf instead of current uart0* functions
* Separate diagnostics from uart, so depending on HW we can do both in different configurations
    * How to do this with printf?
    * We make fprintf and fputc
* Trigger bootloader from CDC like Arduino (1200 BAUD, DTR low)
    * Can we also detect BOSSAC?
* Steering wheel servo pulse is out of range after power on?
    * servo_output_enable should only be activated when gearbox servo configured, to prevent glitch before initialized properly?
* Move stack check into HAL_Service

# PCB

* TLC5940 footprint based on TI datasheet
* Increase spacing of solder bridge to minimum allowed (6mil? 8mil?)
* Increase via size where possible
* Design for stencil 5mil/0.12mm thick