#include <setjmp.h>

/* FIXME(wasm): see setjmp.c. */
_Noreturn void longjmp(jmp_buf env, int val)
{
	for (;;) __builtin_trap();
}

weak_alias(longjmp, _longjmp);
