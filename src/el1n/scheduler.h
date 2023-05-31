/*
 * Copyright (C) 2023 Kian Cross
 */

#ifndef EL1N_SCHEDULER_H_
#define EL1N_SCHEDULER_H_

void el1n_scheduler_start(void *unseal_sentry);
void* el1n_scheduler_save_and_next(void *csp);

#endif
