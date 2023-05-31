/*
 * Copyright (C) 2023 Kian Cross
 */

.global	el0n_init
.type	el0n_init,	"function"
el0n_init:
	// This NOP is needed for extremely hacky reasons. See el1n/scheduler.c for an
	// explanation. We need to be careful that when compiler optimisations are
	// applied, this is not optimised away. At the moment this is not an issue, as
	// optimisations are disabled.
	nop

	// C0 will be a sentry, hence why we need this redirection to call it.
	blr	c0

	// TODO: Instead of looping, we should mark the task as complete (i.e., remove
	// it from the scheduler), so that it is no longer given CPU time.

	// Loop forever after the main programme has finished.
	bl	.

.size	el0n_init,	. - el0n_init
