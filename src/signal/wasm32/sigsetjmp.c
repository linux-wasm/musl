#include <setjmp.h>

/*
 * FIXME(wasm): the compiler only rewrites calls to setjmp(), so this
 * cannot work; see src/setjmp/wasm32/setjmp.c. Use setjmp().
 */
int sigsetjmp(sigjmp_buf env, int savemask)
{
	return 0;
}

weak_alias(sigsetjmp, __sigsetjmp);
