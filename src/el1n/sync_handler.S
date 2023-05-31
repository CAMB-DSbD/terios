/*
 * Copyright (C) 2023 Kian Cross
 */

#define EC_SHIFT 26
#define EC_MASK  (0b111111 << (EC_SHIFT))
#define EC_SVC   0b010101

.global	el1n_sync_handler
.type	el1n_sync_handler,	"function"
el1n_sync_handler:
	// Save value of C0
	str	c0,	[csp, #-16]!

	// Get the value of EC, which contains the reason for the exception.
	mrs	x0,	esr_el1
	and	x0,	x0,	#EC_MASK
	lsr	x0,	x0,	#EC_SHIFT

	// If exception was caused by an SVC, branch to the SVC handler, otherwise
	// infinite loop, as we can't handle the exception.
	cmp	x0,	#EC_SVC
	beq	svc_handler
	b	.

svc_handler:
	// Restore value of C0
	ldr	c0,	[csp],	#16
	b	el1n_svc_handler
