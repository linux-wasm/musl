/* Static linking only: there is no dynamic linker for wasm programs. */
#define LDSO_ARCH "wasm32"
#define CRTJMP(pc,sp) __builtin_trap()
