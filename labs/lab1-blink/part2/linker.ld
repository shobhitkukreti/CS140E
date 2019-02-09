
MEMORY
{
ram  (rwx) : ORIGIN = 0x8000, LENGTH =  0x100000
}


SECTIONS

{
.text : {
     . = ALIGN(4);
     *(.text)
     *(.rodata)
     *(.rodata*)
     . = ALIGN(4);
} > ram
.bss : { *(.bss) } > ram

}