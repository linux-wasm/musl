#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include "syscall.h"
#include "libc.h"
#include "lock.h"
#include "pthread_impl.h"
#include "aio_impl.h"
#include "fork_impl.h"

/*
 * fork() on Linux/wasm.
 *
 * The child has to continue in the middle of the parent's wasm call stack,
 * which is not in memory and cannot be copied from the outside. The
 * program does it itself: it is instrumented (Binaryen's Asyncify pass,
 * applied when the program is linked) so that every function between
 * _start and here can save its locals to a buffer and return, and later
 * be re-entered and restore them. The kernel and its host drive that
 * through the import linux.fork(buffer, size); see wasm_fork() in
 * arch/wasm/kernel/process.c of the kernel.
 *
 * To the code here it is a function that returns twice, as on any other
 * machine. Two things differ:
 *
 *  - The child runs on a new instance of the program, and wasm globals
 *    belong to an instance: the thread pointer is gone in the child. It is
 *    kept in a local across the call (locals are what gets saved) and set
 *    again.
 *  - The buffer bounds the depth of the call stack that can fork; going
 *    over it ends the program with a trap.
 *
 * FIXME(wasm): a program that was not instrumented, and a signal handler,
 * get ENOSYS; so does a program linked without the kernel's fork support.
 */
__attribute__((__import_module__("linux"), __import_name__("fork")))
long __wasm_fork(void *buffer, unsigned long size);

#define FORK_BUFFER_SIZE (256 * 1024)

/* Protected by __abort_lock, which _Fork() holds. */
static void *fork_buffer;

static void dummy(int x) { }
weak_alias(dummy, __aio_atfork);

void __post_Fork(int ret)
{
	if (!ret) {
		pthread_t self = __pthread_self();
		self->tid = __syscall(SYS_set_tid_address, &__thread_list_lock);
		self->robust_list.off = 0;
		self->robust_list.pending = 0;
		self->next = self->prev = self;
		__thread_list_lock = 0;
		libc.threads_minus_1 = 0;
		if (libc.need_locks) libc.need_locks = -1;
	}
	UNLOCK(__abort_lock);
	if (!ret) __aio_atfork(1);
}

pid_t _Fork(void)
{
	pid_t ret;
	sigset_t set;
	uintptr_t tp;

	__block_all_sigs(&set);
	LOCK(__abort_lock);
	if (!fork_buffer) {
		void *p = __mmap(0, FORK_BUFFER_SIZE, PROT_READ|PROT_WRITE,
				 MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if (p != MAP_FAILED) fork_buffer = p;
	}
	tp = __get_tp();
	ret = fork_buffer ? __wasm_fork(fork_buffer, FORK_BUFFER_SIZE) : -ENOMEM;
	__set_thread_area((void *)tp);
	__post_Fork(ret);
	__restore_sigs(&set);
	return __syscall_ret(ret);
}
