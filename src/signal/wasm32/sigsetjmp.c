#include <setjmp.h>

/* FIXME(wasm): see src/setjmp/wasm32/setjmp.c. */
int sigsetjmp(sigjmp_buf env, int savemask)
{
	return 0;
}

weak_alias(sigsetjmp, __sigsetjmp);
