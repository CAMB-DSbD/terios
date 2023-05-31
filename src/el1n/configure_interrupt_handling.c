/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdint.h>

#include "vector_table.h"

static void configure_cvbar(void) {
	void *vector_table = el1n_get_vector_table();

	// Set the vector table address.
	__asm__("msr cvbar_el1, %0" :: "C" (vector_table));
}

static void configure_cctlr() {
	uint64_t cctlr;

	__asm__("mrs %0, cctlr_el1" : "=r" (cctlr));

	// Sets PSTATE.C64 on exception
	cctlr |= 1 << 5; // CCTLR_EL1.C64E = 1

	__asm__("msr cctlr_el1, %0" :: "r" (cctlr));

}

static void enable_all_interrupts() {
	__asm__("msr DAIFClr, #0xF");
}

void el1n_configure_interrupt_handling(void) {
	configure_cvbar();
	configure_cctlr();
	enable_all_interrupts();
}
