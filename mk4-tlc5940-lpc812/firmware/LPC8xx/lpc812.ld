MEMORY
{
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x4000            /* 16K */
  RAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x1000      /* 4K */
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        KEEP(*(.after_vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .data : AT (_etext)
    {
        _data = .;
        *(.survivors)       /* 80 bytes that survive reset in LPC81x */
        *(.data*)
        . = ALIGN(4);
        _edata = .;

    } > RAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    _stacktop = ORIGIN(RAM) + LENGTH(RAM);
}
