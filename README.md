# musl for Linux on WebAssembly

This is [musl](https://musl.libc.org/) (upstream `README` is unchanged) and
the place where its port to the
[linux-wasm kernel](https://github.com/linux-wasm/linux) will live.

**Status: first programs run.** `arch/wasm32` exists; a static `libc.a`
builds with stock clang 19. Exercised on the kernel by the distro's demo
init and `selftest` program: stdio and `printf` (including floating
point), `malloc`, files and directories (with 64-bit offsets), pipes,
`posix_spawn` + `waitpid`, `vfork` (with `_exit` and with `execl`),
`setjmp`/`longjmp`, clocks and `nanosleep`, signals (`sigaction`,
`raise`, `kill`, `sigprocmask`, `setitimer`, `longjmp` out of a handler),
and pthreads (create, join, mutex). No libc test suite has been run;
sockets, locale and most of the rest are untried.

## Building

The supported way is through
[linux-wasm/distro](https://github.com/linux-wasm/distro)
(`scripts/build.sh musl`), which uses the kernel's toolchain container.
By hand, with clang/llvm 19:

```sh
CC="clang --target=wasm32-unknown-unknown" AR=llvm-ar RANLIB=llvm-ranlib \
CFLAGS="-fPIC -matomics -mbulk-memory -mexception-handling -O2" LIBCC=" " \
  ./configure --target=wasm32 --prefix=/ --disable-shared
make && make install DESTDIR=/path/to/sysroot
```

A program is then compiled with `-fPIC -matomics -mbulk-memory
-mexception-handling -mllvm -wasm-enable-sjlj` (the last two make
`setjmp`/`longjmp` work) and linked as a position-independent module:

```sh
wasm-ld -pie --experimental-pic --no-entry \
  --export=_start --export-if-defined=__clone_entry \
  --import-memory --shared-memory --max-memory=4294967296 \
  $SYSROOT/lib/crt1.o prog.o $SYSROOT/lib/libc.a libclang_rt.builtins-wasm32.a
```

The program ABI (imports, `_start`, `__clone_entry`) is defined by the
kernel: see `arch/wasm/kernel/binfmt_wasm.c` in linux-wasm/linux.

## What is stubbed (`FIXME(wasm)` in the source)

- `setjmp`/`longjmp` only work in code compiled with `-mllvm
  -wasm-enable-sjlj`; `sigsetjmp` and `_setjmp` do not work at all (they
  return 0 and a later jump ends the program).
- `sigsetjmp` is not a working `setjmp` (see above), so `siglongjmp` out
  of a signal handler does not work; `setjmp`/`longjmp` does.
- A signal handler runs when a system call returns (the kernel calls it;
  there is no `sa_restorer` and no sigreturn trampoline), so a thread that
  computes without system calls sees no signals. The handler's `ucontext_t`
  has no program counter: `uc_mcontext` describes the interrupted system
  call. `sigaltstack` is accepted and ignored.
- `pthread_cancel` cannot interrupt a thread that is blocked in a system
  call; the thread is cancelled at its next cancellation point.
- `fork()` fails with `EINVAL`: the kernel cannot copy an address space.
  `posix_spawn` and `vfork()` work. `vfork()` is a macro around `setjmp`
  (see `src/process/wasm32/vfork.c`), so its callers need the sjlj compiler
  flags, a vfork child cannot vfork again, and two threads must not vfork
  at the same time.
- `main(int, char **, char **)` (three arguments) does not link.
- No dynamic linking (`--disable-shared` only), no `dlopen`.
- `brk` is not available in the kernel; `malloc` uses `mmap`.

## Layout and workflow

- `master` and the `v*` tags mirror upstream
  (`https://git.musl-libc.org/git/musl`, remote `upstream`).
- `wasm-port` is the development branch and the target for pull requests,
  based on an upstream release tag.
- Commits follow [Conventional Commits](https://www.conventionalcommits.org/)
  in English, e.g. `feat(wasm32): add setjmp`.
- Building and testing against the kernel is driven from
  [linux-wasm/distro](https://github.com/linux-wasm/distro).

## License

MIT, like musl (see `COPYRIGHT`).
