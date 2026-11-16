/* There is no register file on wasm. */
#define ELF_NGREG 1
#define ELF_NFPREG 1
typedef unsigned long elf_greg_t, elf_gregset_t[ELF_NGREG];
typedef struct { unsigned long __unused; } elf_fpregset_t;
