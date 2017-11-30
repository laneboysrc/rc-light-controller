MEMORY
{
    rom (rx)  : ORIGIN = 0x08000000, LENGTH = 32K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 6K
}

/* Put the configuration in the upper-most 8 KBytes of flash */
SECTIONS
{
    /* FIXME: does this work? */
    .persistent_data 0x0800e000 :
    {
        *(.persistent_data)
    }
}


INCLUDE libopencm3_stm32f0.ld