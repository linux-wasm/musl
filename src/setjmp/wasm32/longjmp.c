#include <setjmp.h>

_Noreturn void __wasm_longjmp(void *, int);

/* See setjmp.c. */
_Noreturn void longjmp(jmp_buf env, int val)
{
	__wasm_longjmp(env, val);
}

weak_alias(longjmp, _longjmp);
