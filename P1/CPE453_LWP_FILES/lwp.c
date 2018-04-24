#include "lwp.h"
#include <stdlib.h>
#include <ncurses.h>

int pid = 0;
lwp_context context_to_add;
schedfun my_sched;
ptr_int_t* orginal_stack_pointer;
lwp_context lwp_ptable[LWP_PROC_LIMIT];/* the process table           */
int lwp_procs;           /* the current number of LWPs  */
int lwp_running;         /* the index of the currently running LWP */
int round_robbin();
int stopped = 0;

int new_lwp(lwpfun func, void *args, size_t stack_size) {
	ptr_int_t *sp, *bp, *new_stack;
	if (lwp_procs > LWP_PROC_LIMIT) {
		return -1;
	}	

	lwp_running = lwp_procs;
	lwp_procs++;

	new_stack = malloc(stack_size * sizeof(ptr_int_t));
	context_to_add.stack = new_stack;
	context_to_add.pid = pid;
	context_to_add.stacksize = stack_size;
	pid++;

	sp = new_stack + stack_size;
	sp--;
	*sp = (ptr_int_t)args;
	sp--;
	*sp = (ptr_int_t)lwp_exit;
	sp--;
	*sp = (ptr_int_t)func;
	sp--;
	*sp = (ptr_int_t) 0xAAAAA;
	bp = sp; 	
	sp -= 7;
	*sp = (ptr_int_t)bp;

	context_to_add.sp = sp;
	lwp_ptable[lwp_running] = context_to_add;
	
	return pid;
}

void lwp_stop() {
	SAVE_STATE();
	GetSP(lwp_ptable[lwp_running].sp);
	stopped = lwp_running;
	SetSP(orginal_stack_pointer);
	RESTORE_STATE();
}

int lwp_getpid() {
	return lwp_ptable[lwp_running].pid;
}

void lwp_set_scheduler(schedfun sched) {
	my_sched = sched;
}

void lwp_start() {
	if (lwp_procs == 0) {
		return;
	}

	SAVE_STATE();
	GetSP(orginal_stack_pointer);

	if (stopped > 0) {
		lwp_running = stopped;
	} else if (my_sched == NULL) {
		lwp_set_scheduler(round_robbin);
		lwp_running = my_sched();
	} else {
		lwp_running = my_sched();
	}

	SetSP(lwp_ptable[lwp_running].sp);
	RESTORE_STATE();
}

void lwp_yield() {
	SAVE_STATE();
	GetSP(lwp_ptable[lwp_running].sp);

	if (my_sched == NULL) {
		lwp_set_scheduler(round_robbin);
		lwp_running = my_sched();
	} else {
		lwp_running = my_sched();
	}

	SetSP(lwp_ptable[lwp_running].sp);
	RESTORE_STATE();
}

void lwp_exit() {
	int i = lwp_running;
	free(lwp_ptable[lwp_running].stack);

	for (; i < lwp_procs; i++) {
		lwp_ptable[i] = lwp_ptable[i + 1];
	}

	lwp_procs--;
	lwp_running--;

	if (lwp_procs == 0) {
		lwp_stop();
	} else {
		if (my_sched == NULL) {
			lwp_set_scheduler(round_robbin);
			lwp_running = my_sched();
		} else {
			lwp_running = my_sched();
		}

		SetSP(lwp_ptable[lwp_running].sp);
		RESTORE_STATE();
	}
}

int round_robbin() {
	lwp_running++;
	if (lwp_running == lwp_procs) {
		lwp_running = 0;
	}

	return lwp_running;
}
