/*
 * Fallback for programs whose main takes no arguments; see
 * crt/wasm32/crt1.c. A program with main(int, char **) defines
 * __main_argc_argv itself and this file is not linked.
 */
int __main_void(void);

__attribute__((__weak__))
int __main_argc_argv(int argc, char **argv)
{
	return __main_void();
}
