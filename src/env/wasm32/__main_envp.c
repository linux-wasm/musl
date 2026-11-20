/*
 * Fallback for programs with main(int, char **, char **): clang leaves
 * that one alone, as "main" with three parameters, and defines neither
 * __main_argc_argv nor __main_void. This file is only linked for such a
 * program, to satisfy the reference in __main_void.c.
 */
#include <unistd.h>
#include "libc.h"

int main(int, char **, char **);

extern hidden int __main_argc;
extern hidden char **__main_argv;

int __main_void(void)
{
	return main(__main_argc, __main_argv, __environ);
}
