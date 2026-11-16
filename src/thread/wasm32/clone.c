#define _GNU_SOURCE
#include <sched.h>
#include <stdarg.h>
#include <stdint.h>
#include "pthread_impl.h"
#include "syscall.h"

/*
 * clone() on Linux/wasm never returns in the child: a new thread cannot
 * continue in the middle of the parent's wasm call stack. Instead the
 * kernel starts the child on a fresh instance of the program, at the
 * export __clone_entry, passing the stack pointer given to clone().
 *
 * So everything the child needs is put on its stack first.
 */
struct clone_start {
	int (*func)(void *);
	void *arg;
	uintptr_t tp;
	uintptr_t pad;
};

int __clone(int (*func)(void *), void *stack, int flags, void *arg, ...)
{
	struct clone_start *start;
	va_list ap;
	pid_t *ptid, *ctid;
	void *tls;

	va_start(ap, arg);
	ptid = va_arg(ap, pid_t *);
	tls = va_arg(ap, void *);
	ctid = va_arg(ap, pid_t *);
	va_end(ap);

	start = (void *)(((uintptr_t)stack & -16) - sizeof *start);
	start->func = func;
	start->arg = arg;
	/*
	 * Globals are per instance, so the child starts without a thread
	 * pointer. A new thread gets its own; any other child keeps using
	 * the parent's, as it would after a real clone().
	 */
	start->tp = (flags & CLONE_SETTLS) ? (uintptr_t)tls : __get_tp();

	return __syscall(SYS_clone, flags, start, ptid, tls, ctid);
}

__attribute__((__export_name__("__clone_entry")))
void __clone_entry(struct clone_start *start)
{
	__set_thread_area((void *)start->tp);
	__syscall(SYS_exit, start->func(start->arg));
	for (;;) __builtin_trap();
}
