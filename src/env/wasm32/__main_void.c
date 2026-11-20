/*
 * Fallback for programs whose main takes no arguments; see
 * crt/wasm32/crt1.c. A program with main(int, char **) defines
 * __main_argc_argv itself and this file is not linked.
 *
 * The arguments are kept for the next fallback, __main_envp.c.
 */
#include "libc.h"

int __main_void(void);

hidden int __main_argc;
hidden char **__main_argv;

__attribute__((__weak__))
int __main_argc_argv(int argc, char **argv)
{
	__main_argc = argc;
	__main_argv = argv;
	return __main_void();
}
