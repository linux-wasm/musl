#include <stdlib.h>
#include <unistd.h>
#include "syscall.h"

hidden _Noreturn void __vfork_return(pid_t);

_Noreturn void _Exit(int ec)
{
	/* Only returns in a vfork child, as the parent; see vfork.c. */
	long r = __syscall(SYS_exit_group, ec);

	if (r > 0)
		__vfork_return(r);
	for (;;) __syscall(SYS_exit, ec);
}
