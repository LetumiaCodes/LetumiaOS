ENTRY(kernel_main)

SECTIONS {
  . = 1M;

  .multiboot :
  {
    KEEP(*(.multiboot))
  }

  .text :
  {
    *(.text*)
  }

  .rodata :
  {
    *(.rodata*)
  }

  .data :
  {
    *(.data*)
  }

  .bss :
  {
    *(COMMON)
    *(.bss*)
  }
}
