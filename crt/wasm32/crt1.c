#include <features.h>
#include "libc.h"

#include "crt_arch.h"

/*
 * clang renames main on wasm according to its signature, because wasm
 * functions are typed and a caller must know which one it is calling:
 * main(int, char **) becomes __main_argc_argv and main(void) becomes
 * __main_void. libc supplies a weak __main_argc_argv that calls
 * __main_void (src/env/wasm32/__main_void.c), so both forms link.
 * FIXME(wasm): main(int, char **, char **) is not supported.
 */
int __main_argc_argv(int, char **);

weak void _init();
weak void _fini();
int __libc_start_main(int (*)(int, char **, char **), int, char **,
	void (*)(), void(*)(), void(*)());

/* __libc_start_main calls main with three arguments; types must match. */
static int main3(int argc, char **argv, char **envp)
{
	return __main_argc_argv(argc, argv);
}

hidden void _start_c(long *p)
{
	int argc = p[0];
	char **argv = (void *)(p+1);
	__libc_start_main(main3, argc, argv, _init, _fini, 0);
}
