/*
 * Copyright (C) 2023 Kian Cross
 */

#ifndef EL1N_PARTITION_ROOT_CAP_H_
#define EL1N_PARTITION_ROOT_CAP_H_

struct root_caps {
	void *unseal_sentry;
	unsigned char *gic;
};

void el1n_partition_root_cap(struct root_caps *caps);

#endif
