/* wasm threads proposal: compare-and-swap is a single instruction. */
#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s)
{
	return __sync_val_compare_and_swap(p, t, s);
}

#define a_barrier a_barrier
static inline void a_barrier()
{
	__sync_synchronize();
}

#define a_crash a_crash
static inline void a_crash()
{
	__builtin_trap();
}
