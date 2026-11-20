#include <setjmp.h>
#include <signal.h>

/* See sigsetjmp.c: the mask is restored here, before the jump. */
_Noreturn void siglongjmp(sigjmp_buf env, int val)
{
	if (env->__fl)
		pthread_sigmask(SIG_SETMASK, (sigset_t *)env->__ss, 0);
	longjmp(env, val);
}
