# musl for Linux on WebAssembly

This is [musl](https://musl.libc.org/) (upstream `README` is unchanged) and
the place where its port to the
[linux-wasm kernel](https://github.com/linux-wasm/linux) will live.

**Status: first programs run.** `arch/wasm32` exists; a static `libc.a`
builds with stock clang 19, and C programs have run as `/init` on the
kernel using `printf` (including floating point), `malloc`, `uname`,
`getpid`, `posix_spawn` + `waitpid` (child with arguments and environment,
exit status collected) and `reboot`. Nothing else has been tested: no test
suite has been run, and threads, files, sockets and locale are untried.

## Building

The supported way is through
[linux-wasm/distro](https://github.com/linux-wasm/distro)
(`scripts/build.sh musl`), which uses the kernel's toolchain container.
By hand, with clang/llvm 19:

```sh
CC="clang --target=wasm32-unknown-unknown" AR=llvm-ar RANLIB=llvm-ranlib \
CFLAGS="-fPIC -matomics -mbulk-memory -O2" LIBCC=" " \
  ./configure --target=wasm32 --prefix=/ --disable-shared
make && make install DESTDIR=/path/to/sysroot
```

A program is then compiled with `-fPIC -matomics -mbulk-memory` and linked
as a position-independent module:

```sh
wasm-ld -pie --experimental-pic --no-entry \
  --export=_start --export-if-defined=__clone_entry \
  --import-memory --shared-memory --max-memory=4294967296 \
  $SYSROOT/lib/crt1.o prog.o $SYSROOT/lib/libc.a libclang_rt.builtins-wasm32.a
```

The program ABI (imports, `_start`, `__clone_entry`) is defined by the
kernel: see `arch/wasm/kernel/binfmt_wasm.c` in linux-wasm/linux.

## What is stubbed (`FIXME(wasm)` in the source)

- `setjmp`/`longjmp`/`sigsetjmp`: `setjmp` returns 0, `longjmp` traps.
- Signal handlers never run (kernel limitation), so the signal return
  trampolines are unreachable and `pthread_cancel` cannot interrupt a thread
  that is blocked in a system call.
- `fork()` fails (the kernel requires `CLONE_VM` and an entry function) and
  `vfork()` falls back to it (`EINVAL`). Use `posix_spawn`, which is built
  on `__clone` with an entry function and works.
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
