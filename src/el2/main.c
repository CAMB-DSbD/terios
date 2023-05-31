/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdio.h>

#include "configure_pre_el1n_transition.h"

static void configure_cnthctl(void) {
	uint64_t cnthctl;

	__asm__("mrs %0, cnthctl_el2" : "=r" (cnthctl));

	// Do not trap accesses to CNTP_CTL_EL0, CNTP_CVAL_EL0, and CNTP_TVAL_EL0.
	cnthctl |= (1 << 1); // CNTHCTL_EL2.EL1PCEN = 1

	__asm__("msr cnthctl_el2, %0" :: "r" (cnthctl));
}

static void configure_el2(void) {
	configure_cnthctl();
}

int main() {
	puts("\n");
	puts("Starting teriOS");

	puts("Configuring EL2");
	configure_el2();

	puts("Entering EL1N");
	el2_configure_pre_el1n_transition();

	__asm__(
			"isb;"
			"eret;"
	);

	// Should never reach here.
	return 0;
}
