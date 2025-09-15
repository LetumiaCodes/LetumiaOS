
OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000;  /* Load address (1MB) */

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : { *(.rodata) }

    .data : { *(.data) }

    .bss : { *(.bss) }
}
