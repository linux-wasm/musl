#include <setjmp.h>
#include <signal.h>

/*
 * The compiler only rewrites calls to setjmp() (src/setjmp/wasm32/
 * setjmp.c), so sigsetjmp() is a macro in <setjmp.h>: it calls this to
 * save the signal mask and then setjmp(). siglongjmp() restores the mask
 * before it jumps.
 */
int __sigsetjmp_save(sigjmp_buf env, int savemask)
{
	env->__fl = savemask;
	if (savemask)
		pthread_sigmask(SIG_SETMASK, 0, (sigset_t *)env->__ss);
	return 0;
}

/*
 * FIXME(wasm): for callers that bypass the macro (no header, or the
 * function's address): reports the direct return, and a later jump to it
 * ends the program.
 */
#undef sigsetjmp
int sigsetjmp(sigjmp_buf env, int savemask)
{
	return __sigsetjmp_save(env, savemask);
}

weak_alias(sigsetjmp, __sigsetjmp);
