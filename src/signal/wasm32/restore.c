#include "syscall.h"

/*
 * Signal return trampolines. The kernel does not run signal handlers on
 * wasm yet, so these are never reached.
 */
hidden void __restore_rt(void)
{
	__syscall(SYS_rt_sigreturn);
}

hidden void __restore(void)
{
	__syscall(SYS_rt_sigreturn);
}
