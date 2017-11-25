MEMORY
{
  FLASH (rx)  : ORIGIN = 0x0,        LENGTH = 16k
  RAM   (rwx) : ORIGIN = 0x10000000, LENGTH = 4k
}

ENTRY(Reset_handler)

SECTIONS
{
    .text : ALIGN(4)
    {
        _text = .;
        FILL(0xff)

        KEEP(*(.isr_vectors))

        . = ALIGN(64);
        KEEP(*(.persistent_data))

        . = ALIGN(64);
        *(.text*)
        *(.rodata)
        *(.rodata.*)
        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > FLASH


    .data : ALIGN(4)
    {
        _data = .;
        FILL(0xff)

        *(vtable)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH


    /* Light programs must be located at the end of the flash memory,
     * which is after the RAM initialization data
     (_etext + SIZEOF(.data))
     */
    .light_programs : ALIGN(4)
    {
        *(.light_programs)
    } > FLASH


    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;
    } > RAM


    PROVIDE(_ram = ORIGIN(RAM));
    PROVIDE(_eram = ORIGIN(RAM) + LENGTH(RAM));

    /* The stack starts 32 bytes below to of RAM as those 32 bytes are needed
     * for the IAR ROM function that we use to write the Flash with persistent
     * data.
     */
    PROVIDE(_stacktop = ORIGIN(RAM) + LENGTH(RAM) - 32);
}
