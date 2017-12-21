MEMORY
{
    /* The bootloader occupies the lowest 8 kBytes of the flash */
    FLASH (rx)  : ORIGIN = 0x0,        LENGTH = 8k
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
        _data = .;
        FILL(0x00)

        *(vtable)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

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

