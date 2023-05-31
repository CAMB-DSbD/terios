/*
 * Copyright (C) 2023 Kian Cross
 */

 #include "../svc.h"

.global	el1n_svc_handler
.type	el1n_svc_handler,	"function"
el1n_svc_handler:
	stp	c29,	c30,	[csp, #-32]!
	mov	c29,	csp

	// We currently only support one SVC.
	cmp	x8,	#SVC_ID_PUTS
	beq	puts_handler

	// If the SVC ID is invalid, just return without doing anything.
	b finished

puts_handler:
	bl	puts
	b	finished

finished:
	ldp	c29,	c30,	[csp],	#32
	eret
