/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stddef.h>
#include <cheriintrin.h>

#include "partition_root_cap.h"

static void set_ddc(void *ddc) {
	__asm__("msr ddc, %0" :: "C" (ddc));
}

static void* get_gic(void *ddc) {
	void *gic;

	gic = cheri_address_set(ddc, 0x30000000);
	gic = cheri_bounds_set(gic, 0x2000000);
	gic = cheri_perms_and(gic, CHERI_PERM_LOAD | CHERI_PERM_STORE);

	return gic;
}

static void* get_unseal_sentry(void *ddc) {
	void *unseal_sentry;

	unseal_sentry = cheri_address_set(ddc, 1);
	unseal_sentry = cheri_perms_and(unseal_sentry, CHERI_PERM_UNSEAL);

	return unseal_sentry;
}

void el1n_partition_root_cap(struct root_caps *caps) {
	void *ddc = cheri_ddc_get();

	caps->unseal_sentry = get_unseal_sentry(ddc);
	caps->gic = get_gic(ddc);

	set_ddc(NULL);
}
