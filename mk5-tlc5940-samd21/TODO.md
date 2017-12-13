# Firmware
* Test revised dignostics selection on LPC812
* Merge LPC812 HAL into a single source file
* Trigger bootloader from CDC like Arduino (1200 BAUD, DTR low)
    * Can we also detect BOSSAC?
* Button input via HAL (must be configurable)
* LED out separate IO than OUT15S?

# PCB

* TLC5940 footprint based on TI datasheet
* Increase spacing of solder bridge to minimum allowed (6mil? 8mil?)
* Increase via size where possible
* Design for stencil 5mil/0.12mm thick