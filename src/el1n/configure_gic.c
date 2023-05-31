/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdint.h>

// These constants are taken from the GIC v3/4 manual, GIC-600 manual, and
// Morello system development reference manual.
#define GICD_OFFSET             0
#define GICD_CTLR_OFFSET        ((GICD_OFFSET) + 0)
#define GICD_CTLR_ENABLE_GRP_1A (1 << 1)

#define GICR_OFFSET             0xC0000
#define GICR_SGI_OFFSET         ((GICR_OFFSET) + (64 * 1024))
#define GICR_IGROUPR0_OFFSET    ((GICR_SGI_OFFSET) + 0x80)
#define GICR_IGRPMODR0_OFFSET   ((GICR_SGI_OFFSET) + 0xD00)
#define GICR_ISENABLER0_OFFSET  ((GICR_SGI_OFFSET) + 0x100)

#define CNTP_INTERRUPT_ID       30

static void enable_gic_group_1(unsigned char *gic) {
	*(uint32_t*) (gic + GICD_CTLR_OFFSET) = GICD_CTLR_ENABLE_GRP_1A;

	__asm__("dsb sy");
}

static void enable_interrupts_on_pe(void) {
	uint64_t enable = 1;

	// TODO: Can this be constrained?
	uint64_t pmr_mask = 0xFF;

	__asm__(
			"msr icc_sre_el1, %[enable];"
			"msr icc_pmr_el1, %[pmr_mask];"
			"msr icc_igrpen1_el1, %[enable];"
			"isb;" ::
			[enable] "r" (enable),
			[pmr_mask] "r" (pmr_mask)
	);
}

static void enable_interrupt(unsigned char *gic, short id) {
	uint32_t interrupt_mask = (1 << id);

	*(uint32_t*) (gic + GICR_IGROUPR0_OFFSET) = interrupt_mask;
	*(uint32_t*) (gic + GICR_IGRPMODR0_OFFSET) = interrupt_mask;
	*(uint32_t*) (gic + GICR_ISENABLER0_OFFSET) = interrupt_mask;

	__asm__("dsb sy");
}

void el1n_configure_gic(unsigned char *gic) {
	// TF-A already powers on the redistributor (see gic-x00.c), so no need to
	// do that here. We only need to configure it.

	// Enable group 1 interrupts.
	enable_gic_group_1(gic);

	// Route interrupts to current processing element.
	enable_interrupts_on_pe();

	// Enable timer interrupts.
	enable_interrupt(gic, CNTP_INTERRUPT_ID);
}
