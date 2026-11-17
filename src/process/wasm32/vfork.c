#define _GNU_SOURCE
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include "syscall.h"
#include "libc.h"

/*
 * vfork() on Linux/wasm.
 *
 * A child cannot resume inside its parent's wasm call stack on a thread of
 * its own, so the kernel's wasm_vfork system call lends it the parent's
 * thread: the call returns 0 and the caller simply is the child from then
 * on. When the child calls execve() (successfully) or _exit(), the kernel
 * turns the thread back into the parent and that system call returns the
 * child's pid. execve() and _Exit() then longjmp back to the place where
 * vfork() was called, which the vfork() macro in <unistd.h> marked with
 * setjmp(), and vfork() returns the pid there.
 *
 * FIXME(wasm): one jmp_buf for the whole process, so two threads must not
 * be inside vfork() at the same time; and a vfork child cannot vfork.
 */
static jmp_buf vfork_jb;
static pid_t vfork_pid;

jmp_buf *__vfork_jmp_buf(void)
{
	return &vfork_jb;
}

/* First return of the setjmp(): become the child, or fail as the parent. */
pid_t __vfork_start(void)
{
	return __syscall_ret(__syscall(SYS_wasm_vfork));
}

/* Second return of the setjmp(): back in the parent. */
pid_t __vfork_result(void)
{
	return vfork_pid;
}

/* Called by execve() and _Exit() when the kernel made us the parent again. */
hidden _Noreturn void __vfork_return(pid_t pid)
{
	vfork_pid = pid;
	longjmp(vfork_jb, 1);
}

/* For callers that bypass the macro: there is no function-call vfork. */
#undef vfork
pid_t vfork(void)
{
	errno = ENOSYS;
	return -1;
}
