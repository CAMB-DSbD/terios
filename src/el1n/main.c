/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdio.h>

#include "partition_root_cap.h"
#include "configure_gic.h"
#include "configure_mmu.h"
#include "scheduler.h"
#include "configure_interrupt_handling.h"

void el1n_main(void) {
	// Partition root cap and zero the DDC as soon as possible for security.
	struct root_caps caps = { };
	el1n_partition_root_cap(&caps);

	el1n_configure_mmu();

	// We have to wait until the MMU is configured before we can print, because
	// UART needs mapping.
	puts("Now in EL1N");

	el1n_configure_interrupt_handling();
	el1n_configure_gic(caps.gic);

	el1n_scheduler_start(caps.unseal_sentry);
}
