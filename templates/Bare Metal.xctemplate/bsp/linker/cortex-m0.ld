/*
*
*/

/* Entry Point */
ENTRY(Reset_Handler)


/* Specify the memory areas */
MEMORY
{
  FLASH_ISR             (RX)  : ORIGIN = 0x00000000, LENGTH = 0x00000200
  FLASH_CFG             (RX)  : ORIGIN = 0x00000400, LENGTH = 0x00000010
  FLASH                 (RX)  : ORIGIN = 0x00000410, LENGTH = 32k
  SRAM                  (RW)  : ORIGIN = 0x20000000, LENGTH = 16k
}

__EndOfRam   = ORIGIN(SRAM) + LENGTH(SRAM);

PROVIDE (_sdata   = __data_start__ );  /* start of DATA section */
PROVIDE (_edata   = __data_end__);     /* end of DATA section */
PROVIDE (_estack  = __EndOfRam );      /* end of RAM */


/* Define output sections */
SECTIONS
{
  .isr_vector : ALIGN(4)
  {
    FILL(0xFF)
    __vectors_start = ABSOLUTE(.) ;
    KEEP(*(.isr_vector))     	/* Interrupt vectors */
  } > FLASH_ISR

  .flash_config :
  {
    . = ALIGN(4);
    KEEP(*(.FlashConfig))    /* NXP Flash Configuration Field (FCF) */
    . = ALIGN(4);
  } > m_flash_config

  /* The program code and other data goes into internal flash */
  .text :
  {
    . = ALIGN(4);
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } > m_text

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

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  .data : AT(__DATA_ROM)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;      /* create a global symbol at data start */
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > SRAM

  __DATA_END = __DATA_ROM + (__data_end__ - __data_start__);
  text_end = ORIGIN(FLASH) + LENGTH(FLASH);
  ASSERT(__DATA_END <= text_end, "region FLASH overflowed with text and data")

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    *(.bss)
    *(.bss*)
    . = ALIGN(512);
    *(COMMON)
    . = ALIGN(4);
    __END_BSS = .;
  } > SRAM


  .ARM.attributes 0 : { *(.ARM.attributes) }
}
