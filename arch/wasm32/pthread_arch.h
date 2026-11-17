/*
 * The thread pointer lives in a mutable wasm global. Every thread runs on
 * its own instance of the program, and instances do not share globals, so
 * the global is naturally per-thread.
 */
__asm__(".globaltype __wasm_tp, i32\n");

static inline uintptr_t __get_tp()
{
	uintptr_t tp;
	__asm__ __volatile__("global.get __wasm_tp\n"
			     "local.set %0" : "=r"(tp));
	return tp;
}

#define TLS_ABOVE_TP
#define GAP_ABOVE_TP 0

/*
 * No program counter in the signal context (see bits/signal.h). The
 * cancellation handler compares this with code addresses, which a system
 * call number never matches.
 */
#define MC_PC __syscall_nr
