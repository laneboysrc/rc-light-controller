MEMORY
{
  FLASH (rx)  : ORIGIN = 0,          LENGTH = 32k
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 4k
}

ENTRY(Reset_Handler)

SECTIONS
{
    /* Placeholder for the bootloader binary image */
    .bootloader : ALIGN(4)
    {
        KEEP(*(.bootloader))
        FILL(0xff)
        . = 0x2000; /* 8K bootloader size */
    } > FLASH

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
        KEEP(*(.init))
        KEEP(*(.fini))

        . =  ALIGN(4);
        _etext = .;
    } > FLASH

    .rodata : ALIGN(4)
    {
        *(.rodata)
        *(.rodata.*)
    } > FLASH

    /* Section for libgcc */
    .libgcc : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH

    .data : ALIGN(4)
    {
        _data_load = LOADADDR (.data);
        _data = .;
        FILL(0x00)

        *(vtable)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    /* Light programs must be located at the end of the flash memory
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

