#include <unistd.h>
#include "syscall.h"

hidden _Noreturn void __vfork_return(pid_t);

int execve(const char *path, char *const argv[], char *const envp[])
{
	long r = __syscall(SYS_execve, path, argv, envp);

	/*
	 * A successful execve never returns, except in a vfork child, where
	 * it returns the child's pid: we are the parent again. See vfork.c.
	 */
	if (r > 0)
		__vfork_return(r);
	return __syscall_ret(r);
}
