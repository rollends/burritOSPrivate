MEMORY
{
    ram : ORIGIN = 0x00050000, LENGTH = 31M
    rom : ORIGIN = 0x60000000, LENGTH = 8M
}

SECTIONS
{
    init :
    {
        *(.init*)
        *(.vector*)
    } > ram
    text : /* The actual instructions. */
    {
        *(.text)
        *(.got)
        *(.got.plt)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
    } >ram

    data : /* Initialized data. */
    {
        _DataStart = . ;
        *(.data)
        _DataEnd = . ;
    } >ram

    .bss : /* Uninitialized data. */
    {
        _BssStart = . ;
        *(.bss)
        _BssEnd = . ;
    } >ram

}
