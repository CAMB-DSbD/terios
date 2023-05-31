/*
 * Copyright (C) 2023 Kian Cross
 */

/*
 * We have to do this because relocation seals executable capabilities, and
 * these sealed capabilities cannot be used within ELRs or VBARs. Instead,
 * this function returns an unsealed capability to a handler within its
 * compilation unit (and thus no relocation is required).
 */
.global	el1n_get_init
.type	el1n_get_init,	"function"
el1n_get_init:
	// Get the init address. It's ok to use ADR because el1n_init is only a few
	// instructions away.
	adr	c0,	el1n_init
	ret

el1n_init:
	bl	el1n_main

	// Loop forever after the main programme has finished.
	b	.
