/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdint.h>
#include <stddef.h>
#include <cheriintrin.h>

#include "../helpers.h"
#include "partition_root_cap.h"

// The root level 1 table.
static uint64_t ttb1[512] __attribute__((aligned(4096))) = { 0 };

// The level 2 table used for the first dram section.
static uint64_t ttb2_dram[512] __attribute__((aligned(4096))) = { 0 };

static void configure_mair(void) {
	// Set Attr1 to:
	// Normal memory, Inner Write-Back Non-transient, Allocate
	// Normal memory, Outer Write-Back Non-transient, Allocate
	// All other AttrIndx values are zeroed (i.e., Device-nGnRnE memory)
	uint64_t mair = 0b110110;

	__asm__("msr mair_el1, %0" :: "r" (mair));
}

static void set_ttbr0(void *ttb1) {
	uint64_t ttb_addr = cheri_address_get(ttb1);

	__asm__("msr ttbr0_el1, %0" :: "r" (ttb_addr));
}

static void configure_peripherals(uint64_t *ttb1) {
	// Peripherals are at the start of the address space, starting at
	// 0x00_2A00_0000. The last peripheral we care about is the GIC, which does
	// not go past 0x00_3200_0000. So we can just map a 1GB block of device
	// memory at the start of the address space, and it will cover the
	// peripherals we need. This is quite crude and can be improved using level
	// 2/3 tables to add additional granularity.
	uint64_t peripherals = 1;

	// [4:2] is the AttrIndx (indexing into mair_el1). Set to 1 to use
	// device memory.
	BIT_SET(peripherals, 2);

	// Allow TLB caching
	BIT_SET(peripherals, 10);

	*(ttb1) = peripherals;
}

static void configure_dram(uint64_t *ttb1, uint64_t *ttb2_dram) {
	uint64_t lower_table_descriptor = 0;

	// 0b11 = table descriptor
	BITS_SET(lower_table_descriptor, 0, 0b11);

	// Set address of level 2 table.
	BITS_SET(lower_table_descriptor, 0, cheri_address_get(ttb2_dram));

	uint64_t dram_attributes = 0;

	// 0b1 = block descriptor.
	BITS_SET(dram_attributes, 0, 0b1);

	// Allow TLB caching
	BIT_SET(dram_attributes, 10);

	// PBHA bits. No effect when storing and loading capabilities.
	BIT_SET(dram_attributes, 60);
	BIT_SET(dram_attributes, 61);

	for (size_t i = 0; i < 512; i++) {
		// Iterate in 2MB increments, using an offset of 0x80000000.
		uint64_t lower_block_descriptor = 0x80000000 + (i * 2 * 1024 * 1024);

		// Past this point, all DRAM (including the upper block) are accessible
		// at EL0. The reason we have to split lower block into two is because
		// lower block needs to be executable, and giving EL0 access to lower
		// blocks means EL1 cannot execute same memory. See link.ld for a more
		// comprehensive explanation.
		if (i == 256) {
			// [7:6] is the AP (access permissions). Set to bit 6 to 1 to allow
			// EL0 to access memory. This allows EL0 access to the stack region,
			// with protections enforced by capabilities rather than the MMU.
			BIT_SET(dram_attributes, 6);
		}

		lower_block_descriptor |= dram_attributes;
		*(ttb2_dram + i) = lower_block_descriptor;
	}

	uint64_t upper_block_descriptor = 0xC0000000 | dram_attributes;

	*(ttb1 + 2) = lower_table_descriptor;
	*(ttb1 + 3) = upper_block_descriptor;
}

static void configure_ttb(void) {
	set_ttbr0(ttb1);
	configure_peripherals(ttb1);
	configure_dram(ttb1, ttb2_dram);

	__asm__("dsb sy");
}

static void configure_tcr(void) {
	uint64_t tcr = 0;

	// The size offset of the memory region addressed by TTBR0_EL1.
	// 64 - 25 = 40 bits.
	BITS_SET(tcr, 0, 25);

	// Normal memory, Inner Write-Back Read-Allocate No Write-Allocate Cacheable.
	BITS_SET(tcr, 8, 0b11);

	// Normal memory, Outer Write-Back Read-Allocate No Write-Allocate Cacheable.
	BITS_SET(tcr, 10, 0b11);

	// A TLB miss on an address that is translated using TTBR1_EL1 generates a
	// Translation fault. No translation table walk is performed.
	BIT_SET(tcr, 23);

	// Hierarchical permissions are disabled.
	BIT_SET(tcr, 41);

	// Allow use of hardware defined bits in descriptor entries.
	BIT_SET(tcr, 43);
	BIT_SET(tcr, 44);
	BIT_SET(tcr, 45);
	BIT_SET(tcr, 46);

	__asm__(
			"msr tcr_el1, %0;"
			"isb;" ::
			"r" (tcr)
	);
}

static void configure_sctlr(void) {
	uint64_t sctlr;

	__asm__("mrs %0, sctlr_el1; isb;" : "=r" (sctlr));

	// This configuration of SCTLR_EL1 mirrors that of SCTLR_EL2 from crt0.S

	// Stage 1 address translation enabled.
	BIT_SET(sctlr, 0); // SCTLR_EL1.M  = 1

	// Enable SP alignment check.
	BIT_SET(sctlr, 3); // SCTLR_EL1.SA = 1

	// Alignment fault checking disabled.
	BIT_CLR(sctlr, 1); // SCTLR_EL1.A = 0

	// No effect on memory access/cacheability
	BIT_SET(sctlr, 2);  // SCTLR_EL1.C = 1
	BIT_SET(sctlr, 12); // SCTLR_EL1.I = 1
	BIT_CLR(sctlr, 19); // SCTLR_EL1.WXN = 0

	__asm__(
			"msr sctlr_el1, %0;"
			"isb;"
			"dsb sy;" ::
			"r" (sctlr)
	);
}

void el1n_configure_mmu(void) {
	configure_mair();
	configure_ttb();
	configure_tcr();
	configure_sctlr();

	__asm__(
			"tlbi vmalle1is;"
			"dsb sy;"
			"isb;"
	);
}
