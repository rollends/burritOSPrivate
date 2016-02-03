MEMORY
{
    ram : ORIGIN = 0x0, LENGTH = 0x10000
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
