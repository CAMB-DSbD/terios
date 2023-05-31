/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdint.h>
#include <stddef.h>

static void configure_spsr(void) {
	uint64_t spsr = 0;

	// Enable c64 (i.e., enable Morello instructions)
	spsr |= 1 << 26; // SPSR_EL1.C64 = 1

	__asm__("msr spsr_el1, %0" :: "r" (spsr));
}

static void configure_ddc(void) {
	__asm__(
			"msr ddc_el0, czr;"
			"msr rddc_el0, czr;"
	);
}

void el1n_configure_pre_el0n_transition(void) {
	configure_spsr();
	configure_ddc();
}
