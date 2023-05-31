/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdint.h>

#include "../helpers.h"
#include "../el1n/init.h"

static void configure_sctlr(void) {
	uint64_t sctlr;

	__asm__(
			// SCTLR_EL1 has an unknown reset value and must be configured
			// before we can enter EL1. Source:
			// https://developer.arm.com/documentation/102437/0100/Changing-Exception-levels
			"msr sctlr_el1, xzr;"
			"mrs %0, sctlr_el1;" ::
			"r" (sctlr)
	);

	// The value of PSTATE.PAN is left unchanged on taking an exception to EL1.
	sctlr |= 1 << 23; // SCTLR_EL1.SPAN = 1

	__asm__("msr sctlr_el1, %0" : "=r" (sctlr));
}

static void configure_cpacr(void) {
	uint64_t cpacr;

	__asm__("mrs %0, cpacr_el1;" : "=r" (cpacr));

	// Do not trap Morello instructions.
	cpacr |= 0b11 << 18; // CPACR_EL1.CEN  = 0b11

	// Do not trap floating point instructions.
	cpacr |= 0b11 << 20; // CPACR_EL1.FPEN = 0b11

	__asm__("msr cpacr_el1, %0" :: "r" (cpacr));
}

static void configure_celr(void) {
	void *init = el1n_get_init();

	__asm__("msr celr_el2, %0" :: "C" (init));
}

static void configure_spsr(void) {
	uint64_t spsr = 0;

	// Use SP = CSP_EL1 at EL1
	BIT_SET(spsr, 0); // SPSR_EL2.M[0] = 1

	// ERET to EL1
	BIT_SET(spsr, 2); // SPSR_EL2.M[3:2] = 1

	// Enable c64 (i.e., enable Morello instructions)
	BIT_SET(spsr, 26); // SPSR_EL2.C64 = 1

	__asm__("msr spsr_el2, %0" :: "r" (spsr));
}

static void configure_csp(void) {
	void *csp;

	__asm__("mov %0, csp" : "=C" (csp));

	// Set the EL1 stack pointer to the same as EL2.
	__asm__("msr csp_el1, %0" :: "C" (csp));
}

void el2_configure_pre_el1n_transition(void) {
	configure_sctlr();

	// We need to make changes to this before the eret, otherwise Morello
	// instructions are not enabled and PSTATE.C64 is not copied from
	// SPSR_EL2.C64.
	configure_cpacr();

	configure_celr();
	configure_spsr();
	configure_csp();
}
