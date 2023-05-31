/*
 * Copyright (C) 2023 Kian Cross
 */

MEMORY
{
	/*
	 * There are 2GB of DRAM (known as DRAM0), one block starting at 0x80000000,
	 * and another at 0xC0000000. We split the first block into two 512MB
	 * sections. The first is used for non-EL0 code, and the second is for EL0
	 * code. The reason for this is because the MMU prevents memory mapped to
	 * both EL0 and EL1 from being executable at EL1, so we have to put EL1 code
	 * somewhere on its own and make sure this region is NOT mapped to EL0.
	*/
	elx_dram (rwx) : ORIGIN = 0x80000000, LENGTH = 0x20000000 
	el0_dram (rwx) : ORIGIN = 0xA0000000, LENGTH = 0x20000000
}

SECTIONS
{
	/* Put EL0 code into its own memory region. */
	.el0_text : {
		*/el0n/*(.text*)
	} > el0_dram

	.el0_data : {
		*/el0n/*(.data*)
	} > el0_dram

	.el0_bss : {
		*/el0n/*(.bss*)
	} > el0_dram

	.el0_rodata : {
		*/el0n/*(.rodata*)
	} > el0_dram

	.text : {
		*(.text*)
	} > elx_dram

	.data : {
		*(.data*)
	} > elx_dram

	.bss : {
		/* These labels are needed by Newlib */
		__bss_start__ = .;
		*(.bss*)
		__bss_end__ = .;
	} > elx_dram

	.rodata : {
		*(.rodata*)
	} > elx_dram

	.rela : {
		*(.rela*)
	} > elx_dram

	__end__ = .;
}
