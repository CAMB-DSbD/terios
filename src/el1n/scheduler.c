/*
 * Copyright (C) 2023 Kian Cross
 */

#include <stdint.h>
#include <stddef.h>
#include <cheriintrin.h>

#include "configure_pre_el0n_transition.h"
#include "timer_interrupt_handler.h"

#include "../el0n/init.h"
#include "../el0n/example_task_1.h"
#include "../el0n/example_task_2.h"
#include "../el0n/example_task_3.h"

#define TASK_STACK_SIZE         (32 * 1024) // 32KB
#define NUMBER_OF_GP_REGISTERS  31

struct task {
	// Entry point of the task.
	void (*entry)(void);

	// Stores the stack pointer of the task. This is where registers are saved
	// during context switches.
	void *csp;
};

// Index into `tasks` of the currently executing task.
static size_t current_task = 0;

static struct task tasks[] = {
		{ .entry = &el0n_example_task_1, .csp = NULL },
		{ .entry = &el0n_example_task_2, .csp = NULL },
		{ .entry = &el0n_example_task_3, .csp = NULL },
};

static size_t get_tasks_length(void) {
	return (sizeof(tasks) / sizeof(struct task));
}

static size_t get_next_task(void) {
	if (current_task == get_tasks_length() - 1) {
		return 0;

	} else {
		return current_task + 1;
	}
}

static void* get_stack_pointer(void) {
	void *csp;

	__asm__("mov %0, csp" : "=C" (csp));

	return csp;
}

/*
 * This is very hacky. We need a non-sealed capability to put into the ELR.
 * However, the dynamic relocation always provides us with a sentry, which
 * also is (inexplicably) offset by 1 from the actual start of the function.
 * This is not a problem when using instructions such as BL, as they unseal
 * and manage the offset. But it is a problem for the ELR.
 *
 * So, here we:
 *
 *   1) Unseal the capability.
 *   2) Re-align to 4 bytes.
 *
 * The problem with realigning is that the first instruction of the function
 * is missed. To fix this, the el0n_init has a NOP as first instruction.
 *
 * Perhaps a nicer solution might be to use the link script to force the
 * location of the el0n_init to its own section. We can then use a label in the
 * link script to get the start address of the section. I have not tried this,
 * and given that this works, I probably won't...
 */
static void* get_el0n_init(void *unseal_sentry) {
	void *init = cheri_unseal(&el0n_init, unseal_sentry);

	uintptr_t init_addr = cheri_address_get(init);

	// Align address to 4-byte boundary.
	init_addr = (init_addr + 3) & ~((uintptr_t) 3);

	init = cheri_address_set(init, init_addr);

	return init;
}

/*
 * Constructs a dummy stack, which can be used in the second half (restoring
 * half) of a context switch. The first 30 values are GP registers, which are
 * set to NULL.
 *
 * The last register to be pushed, c0, is set to the entry function. The final
 * value to be pushed to the stack is the value to be placed into the ELR.
 *
 * So ERET will go to el0n_init, which will use c0 to branch to the actual main
 * function.
 */
static void* initialise_stack(void **csp, void *entry, void *init) {
	for (size_t i = 0; i < NUMBER_OF_GP_REGISTERS - 1; i++) {
		*(--csp) = NULL;
	}

	*(--csp) = entry;
	*(--csp) = init;

	return csp;
}

static void initialise_stack_pointers(void *unseal_sentry) {
	size_t tasks_length = get_tasks_length();
	void *csp = get_stack_pointer();
	void *init = get_el0n_init(unseal_sentry);

	for (size_t i = 0; i < tasks_length; i++) {

		// Leaves a TASK_STACK_SIZE for the previous stack (either the EL1
		// stack, or the previous task's EL0 stack).
		csp -= (2 * TASK_STACK_SIZE);

		// Construct a new capability to represent the contrained stack.
		void *task_csp = cheri_bounds_set(csp, TASK_STACK_SIZE);

		// Back to the start of the stack (remember, it grows downwards).
		task_csp += TASK_STACK_SIZE;

		// Initialise the stack. This function returns the head of the
		// initialised stack.
		task_csp = initialise_stack(task_csp, tasks[i].entry, init);

		tasks[i].csp = task_csp;
	}
}

void el1n_scheduler_start(void *unseal_sentry) {
	initialise_stack_pointers(unseal_sentry);

	el1n_configure_pre_el0n_transition();

	el1n_restore_context(tasks[current_task].csp);
}

void* el1n_scheduler_save_and_next(void *csp) {
	tasks[current_task].csp = csp;

	current_task = get_next_task();

	return tasks[current_task].csp;
}
