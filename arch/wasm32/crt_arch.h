/*
 * The kernel enters a program through its "_start" export with one
 * argument: the address of the argc/argv/envp/auxv block, which is also the
 * initial stack pointer. No assembly needed; just forward to _start_c.
 */
void _start_c(long *);

__attribute__((__export_name__("_start")))
void _start(long *p)
{
	_start_c(p);
}
