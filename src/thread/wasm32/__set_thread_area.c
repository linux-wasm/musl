#include "pthread_impl.h"

/* Definition of the thread pointer global, see arch/wasm32/pthread_arch.h. */
__asm__(".globl __wasm_tp\n"
	".globaltype __wasm_tp, i32\n"
	"__wasm_tp:\n");

int __set_thread_area(void *p)
{
	__asm__ __volatile__("local.get %0\n"
			     "global.set __wasm_tp" : : "r"(p));
	return 0;
}
