/*
 * Copyright (C) 2023 Kian Cross
 */

#include "svc.h"

void el0n_example_task_2(void) {
	for (int i = 0; i < 500; i++) {
		eln0_svc_puts("EL0N Task 2");
	}
}
