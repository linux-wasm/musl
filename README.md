# musl for Linux on WebAssembly

This is [musl](https://musl.libc.org/) (upstream `README` is unchanged) and
the place where its port to the
[linux-wasm kernel](https://github.com/linux-wasm/linux) will live.

**Status: not started.** The `wasm-port` branch is upstream musl `v1.2.6`
plus this file. There is no `arch/wasm32` yet and nothing here builds for
wasm.

## Why nothing is here yet

A libc port targets the kernel's user-space ABI, and the kernel does not
have one yet: it boots but cannot run user programs. These have to be
defined on the kernel side first:

- how a system call enters the kernel from a wasm user program
- the executable format and how the kernel loads it (a wasm binfmt)
- how user threads, `vfork`/`clone` and signals map onto host threads
- the process startup contract (arguments, environment, auxv, stack)

The earlier ports ([tombl/musl](https://github.com/tombl/musl) and the musl
patch in [joelseverin/linux-wasm](https://github.com/joelseverin/linux-wasm))
each target their own kernel's ABI and cannot be used unchanged.

## Layout and workflow

- `master` and the `v*` tags mirror upstream
  (`https://git.musl-libc.org/git/musl`, remote `upstream`).
- `wasm-port` is the development branch and the target for pull requests,
  based on an upstream release tag.
- Commits follow [Conventional Commits](https://www.conventionalcommits.org/)
  in English, e.g. `feat(wasm32): add syscall entry`.
- Building and testing against the kernel is driven from
  [linux-wasm/distro](https://github.com/linux-wasm/distro).

## License

MIT, like musl (see `COPYRIGHT`).
