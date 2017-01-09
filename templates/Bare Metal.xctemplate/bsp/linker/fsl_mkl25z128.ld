/*
 * Linker file for the GNU C Compiler / Binutils
 *
 * Processor: MKL25Z128VFM4
 *            MKL25Z128VFT4
 *            MKL25Z128VLH4
 *            MKL25Z128VLK4
 */


/* Entry Point */
ENTRY(Reset_Handler)

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x0400;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x0400;

/* Specify the memory areas, 128K Flash, 16K SRAM */
MEMORY
{
  FLASH_ISR        (RX)  : ORIGIN = 0x00000000, LENGTH = 0x00000200
  FLASH_CFG        (RX)  : ORIGIN = 0x00000400, LENGTH = 0x00000010
  FLASH            (RX)  : ORIGIN = 0x00000410, LENGTH = 0x0001FBF0
  RAM              (RW)  : ORIGIN = 0x20000000 - 16K / 4, LENGTH = 16K

  /* SRAM_L - 0x1FFF_F000-0x1FFF_FFFF */
  /* SRAM_H - 0x2000_0000-0x2000_2FFF */
}


/*
 * The '__stack' definition is required by crt0, do not remove it.
 */
__stack = ORIGIN(RAM) + LENGTH(RAM);



/* Define output sections */
SECTIONS
{
  /* the .isr_vector section, which goes to FLASH. */
  .isr_vector : ALIGN(4)
  {
    FILL(0xFF)
    __vectors_start = ABSOLUTE(.) ;
    KEEP(*(.isr_vector))
  } >FLASH_ISR

  /* Flash Configuration Field (FCF) */
  .flash_config :
  {
    . = ALIGN(4);
    KEEP(*(.FlashConfig))
    . = ALIGN(4);
  } > FLASH_CFG

  /* The program code and other data goes into internal flash */
  .text : ALIGN(4)
  {
    . = ALIGN(4);
    *(.text)         /* .text sections (code) */
    *(.text*)        /* .text* sections (code) */
    *(.rodata)       /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)      /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)       /* ARM code calling thumb code */
    *(.glue_7t)      /* thumb code calling ARM code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } >FLASH


  /* ARM magic sections */
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > FLASH

  __etext = .;    /* define a global symbol at end of code */
  __DATA_ROM = .; /* Symbol is used by startup for data initialization */


  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > RAM


  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > RAM

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
  } > RAM

  /* Initializes stack on the end of block */
  __StackTop   = ORIGIN(RAM) + LENGTH(RAM);
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  .ARM.attributes 0 : { *(.ARM.attributes) }

  ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack and heap")
}
