MEMORY
{
  FLASH (rx)  : ORIGIN = 0x0,        LENGTH = 32k
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 4k
}

ENTRY(Reset_Handler)

SECTIONS
{
    .text : ALIGN(4)
    {
        _text = .;
        FILL(0xff)

        KEEP(*(.isr_vectors))

        /* Page size is 64 bytes, Row size is 4 pages = 256 bytes */
        . = ALIGN(256);
        KEEP(*(.persistent_data*))
        . = ALIGN(256);

        . = ALIGN(4);
        *(.text*)
        *(.rodata)
        *(.rodata.*)

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

    PROVIDE(_stacktop = ORIGIN(RAM) + LENGTH(RAM));
}

