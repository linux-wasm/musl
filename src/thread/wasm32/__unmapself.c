#include "pthread_impl.h"
#include "syscall.h"

/*
 * The wasm call stack is not in linear memory, so a thread can unmap its
 * own (data) stack and exit as long as nothing touches memory in between:
 * these two calls only pass values.
 */
void __unmapself(void *base, size_t size)
{
	__syscall(SYS_munmap, base, size);
	__syscall(SYS_exit);
	for (;;) __builtin_trap();
}
