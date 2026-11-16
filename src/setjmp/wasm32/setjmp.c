#include <setjmp.h>

/*
 * FIXME(wasm): setjmp/longjmp are not implemented. On wasm they need the
 * compiler's exception-based lowering (-mllvm -wasm-enable-sjlj) and its
 * runtime. Until then setjmp reports the direct return and longjmp traps.
 */
int setjmp(jmp_buf env)
{
	return 0;
}

weak_alias(setjmp, _setjmp);
weak_alias(setjmp, __setjmp);
