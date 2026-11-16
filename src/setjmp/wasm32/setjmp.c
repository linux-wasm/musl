#include <setjmp.h>
#include <stdint.h>

/*
 * setjmp/longjmp on wasm.
 *
 * A wasm function cannot be re-entered from the outside, so the compiler
 * does the work: with "-mexception-handling -mllvm -wasm-enable-sjlj" clang
 * rewrites every function that calls setjmp(). The call becomes
 * __wasm_setjmp(), which records in the jmp_buf which invocation of which
 * function it belongs to and a label for the call site, and calls made
 * afterwards are wrapped in a handler for the __c_longjmp exception.
 * longjmp() throws that exception; each handler on the way up asks
 * __wasm_setjmp_test() whether the jmp_buf is its own and either resumes at
 * the label or rethrows.
 *
 * These are the runtime functions that scheme needs (the same contract as
 * in wasi-libc and Emscripten).
 *
 * A translation unit compiled WITHOUT those flags calls the plain setjmp()
 * below instead. FIXME(wasm): that reports the direct return and a later
 * longjmp() to it ends the program with an uncaught exception.
 * sigsetjmp() and _setjmp() are not recognised by the compiler and behave
 * the same way.
 */
struct wasm_jmp_buf {
	void *invocation;
	uint32_t label;
	/* What the exception carries: where to and with which value. */
	struct {
		void *env;
		int val;
	} arg;
};

void __wasm_setjmp(void *env, uint32_t label, void *invocation)
{
	struct wasm_jmp_buf *jb = env;

	jb->invocation = invocation;
	jb->label = label;
}

uint32_t __wasm_setjmp_test(void *env, void *invocation)
{
	struct wasm_jmp_buf *jb = env;

	return jb->invocation == invocation ? jb->label : 0;
}

_Noreturn void __wasm_longjmp(void *env, int val)
{
	struct wasm_jmp_buf *jb = env;

	jb->arg.env = env;
	jb->arg.val = val ? val : 1;
	/* Tag 1 is the C longjmp tag, __c_longjmp (see longjmp_tag.S). */
	__builtin_wasm_throw(1, &jb->arg);
}

int setjmp(jmp_buf env)
{
	((struct wasm_jmp_buf *)env)->invocation = 0;
	return 0;
}

weak_alias(setjmp, _setjmp);
weak_alias(setjmp, __setjmp);
