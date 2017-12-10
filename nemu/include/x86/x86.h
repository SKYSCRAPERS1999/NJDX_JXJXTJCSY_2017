#ifndef __X86_H__
#define __X86_H__

// CPU rings
#define DPL_KERN  0x0     // Kernel (ring 0)
#define DPL_USER  0x3     // User (ring 3)

// Eflags register
#define FL_TF     0x00000100  // Trap Flag
#define FL_IF     0x00000200  // Interrupt Enable

// System segment type bits
#define STS_T32A  0x9     // Available 32-bit TSS
#define STS_IG32  0xE     // 32-bit Interrupt Gate
#define STS_TG32  0xF     // 32-bit Trap Gate

// Control Register flags
#define CR0_PE    0x00000001  // Protection Enable
#define CR0_PG    0x80000000  // Paging

// Page directory and page table constants
#define NR_PDE    1024    // # directory entries per page directory
#define NR_PTE    1024    // # PTEs per page table
#define PGSHFT    12      // log2(PGSIZE)
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address

// Page table/directory entry flags
#define PTE_P     0x001     // Present
#define PTE_W     0x002     // Writeable
#define PTE_U     0x004     // User
#define PTE_PWT   0x008     // Write-Through
#define PTE_PCD   0x010     // Cache-Disable
#define PTE_A     0x020     // Accessed
#define PTE_D     0x040     // Dirty

// GDT entries
#define NR_SEG    6       // GDT size
#define SEG_KCODE   1       // Kernel code
#define SEG_KDATA   2       // Kernel data/stack
#define SEG_UCODE   3       // User code
#define SEG_UDATA   4       // User data/stack
#define SEG_TSS   5       // Global unique task state segement

#define KSEL(desc) (((desc) << 3) | DPL_KERN)
#define USEL(desc) (((desc) << 3) | DPL_USER)

// The following macros will not be seen by the assembler
#ifndef __ASSEMBLER__

#include "x86/arch.h"

// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(va) --/ \--- PTX(va) --/\------ OFF(va) ------/
#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)

// construct virtual address from indexes and offset
#define PGADDR(d, t, o) ((uint32_t)((d) << PDXSHFT | (t) << PTXSHFT | (o)))

// Address in page table or page directory entry
#define PTE_ADDR(pte)   ((uint32_t)(pte) & ~0xfff)

#endif
#endif
