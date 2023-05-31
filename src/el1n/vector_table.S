/*
 * Copyright (C) 2023 Kian Cross
 */

#define entry(name) .align 7; name:
#define dummy_entry(name) entry(name); b .
#define \
	dummy_block(prefix) \
	dummy_entry(prefix ## _sync); \
	dummy_entry(prefix ## _irq); \
	dummy_entry(prefix ## _fiq); \
	dummy_entry(prefix ## _serror);

/*
 * We have to do this because relocation seals executable capabilities, and
 * these sealed capabilities cannot be used within ELRs or VBARs. Instead,
 * the function returns an unsealed capability to a handler within its
 * compilation unit (and thus no relocation is required).
 */
.global	el1n_get_vector_table
.type	el1n_get_vector_table,	"function"
el1n_get_vector_table:
	// It's ok to use ADR because vector_table is only a few instructions away.
	adr	c0, vector_table
	ret

.align	12
vector_table:
// From current exception level.
dummy_block(cel_spo)
dummy_block(cel_spx)

// From lower exception level (AArch64)
entry(lel_cpx_sync)
	b	el1n_sync_handler
entry(lel_cpx_irq)
	// Only timer interrupts are enabled, so no need to do any routing or
	// filtering here, we an just call the timer interrupt handler immediately.
	b	el1n_timer_interrupt_handler
dummy_entry(lel_cpx_fiq)
dummy_entry(lel_cpx_serror)

// From lower exception level (AArch32)
dummy_block(lel_aarch32)
