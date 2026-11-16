#include "pthread_impl.h"
#include "syscall.h"

/*
 * Cancellation points. The generic scheme inspects the interrupted program
 * counter in a signal handler; wasm has no program counter to inspect, so
 * only the check before the system call exists.
 * FIXME(wasm): a thread already blocked in a system call is not cancelled
 * until the call returns.
 */
hidden const char __cp_begin[1], __cp_end[1], __cp_cancel[1];

hidden long __cancel();

hidden long __syscall_cp_asm(volatile int *cancel, syscall_arg_t nr,
			     syscall_arg_t u, syscall_arg_t v, syscall_arg_t w,
			     syscall_arg_t x, syscall_arg_t y, syscall_arg_t z)
{
	if (*cancel)
		return __cancel();
	return __syscall(nr, u, v, w, x, y, z);
}
