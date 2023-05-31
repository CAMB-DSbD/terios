/*
 * Copyright (C) 2023 Kian Cross
 */

.global el1n_timer_interrupt_handler
.type	el1n_timer_interrupt_handler,	"function"
el1n_timer_interrupt_handler:
	// Save C0 to the EL1 stack and then save the EL0 stack to C0. C0 can now be
	// used to save the task's registers to its own stack.
	str	c0,	[csp,	#-16]!
	mrs	c0,	csp_el0

	// Save the rest of the stack
	stp	c29,	c30,	[c0, #-32]!
	stp	c27,	c28,	[c0, #-32]!
	stp	c25,	c26,	[c0, #-32]!
	stp	c23,	c24,	[c0, #-32]!
	stp	c21,	c22,	[c0, #-32]!
	stp	c19,	c20,	[c0, #-32]!
	stp	c17,	c18,	[c0, #-32]!
	stp	c15,	c16,	[c0, #-32]!
	stp	c13,	c14,	[c0, #-32]!
	stp	c11,	c12,	[c0, #-32]!
	stp	c9,	c10,	[c0, #-32]!
	stp	c7,	c8,	[c0, #-32]!
	stp	c5,	c6,	[c0, #-32]!
	stp	c3,	c4,	[c0, #-32]!
	stp	c1,	c2,	[c0, #-32]!

	// Load the value of C0 into C1 (which is already saved to the EL0 stack),
	// and then save C0 to the EL0 stack too.
	ldr	c1,	[csp],	#16
	str	c1,	[c0, #-16]!

	// Save the EL0 PC to the EL0 stack.
	mrs	c1,	celr_el1
	str	c1,	[c0, #-16]!

	// Read interrupt ID.
	mrs	x19,	icc_iar1_el1

	// C0 contains the EL0 stack, which is passed to this function. The function
	// returns a new EL0 stack to C0 for the next task.
	bl	el1n_scheduler_save_and_next

	// Mark interrupt handler as complete. It is not quite complete, but we need
	// to restore the stack now, so we need to overwrite X19 which means there is
	// nowhere to store the ID.
	msr	icc_eoir1_el1,	x19

// Putting this label half way through the function is a bit of a hack. It
// means we can jump here when we first start the scheduler.
.global	el1n_restore_context
.type	el1n_restore_context,	"function"
el1n_restore_context:
	// (Re)start the timer
	mov	x1,	#0x01
	mov	x2,	#1
	msr	cntp_tval_el0,	x1
	msr	cntp_ctl_el0,	x2

	// Everything that happens here is the reverse of the above. i.e., restore the
	// registers and then the EL0 stack.
	ldr	c1, [c0],	#16
	msr	celr_el1,	c1

	ldr	c1,	[c0],	#16
	str	c1,	[csp, #-16]!

	ldp	c1,	c2,	[c0],	#32
	ldp	c3,	c4,	[c0],	#32
	ldp	c5,	c6,	[c0],	#32
	ldp	c7,	c8,	[c0],	#32
	ldp	c9,	c10,	[c0],	#32
	ldp	c11,	c12,	[c0],	#32
	ldp	c13,	c14,	[c0],	#32
	ldp	c15,	c16,	[c0],	#32
	ldp	c17,	c18,	[c0],	#32
	ldp	c19,	c20,	[c0],	#32
	ldp	c21,	c22,	[c0],	#32
	ldp	c23,	c24,	[c0],	#32
	ldp	c25,	c26,	[c0],	#32
	ldp	c27,	c28,	[c0],	#32
	ldp	c29,	c30,	[c0],	#32

	msr	csp_el0,	c0
	ldr	c0,	[csp],	#16

	isb
	eret
