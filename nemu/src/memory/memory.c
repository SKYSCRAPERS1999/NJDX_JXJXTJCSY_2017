#include "nemu.h"
#include "x86/x86.h"
#define PMEM_SIZE (128 * 1024 * 1024)
#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];
int is_mmio(paddr_t);
uint32_t mmio_read(paddr_t, int, int);
void mmio_write(paddr_t, int, uint32_t, int); 

/* Memory accessing interfaces */

uint32_t page_translate(vaddr_t addr) {
	uint32_t pde_base = cpu.cr3.page_directory_base;
	uint32_t pde_off = PDX(addr);
	uint32_t pde = paddr_read(pde_base + 4 * pde_off, 4);
	assert((pde&1) == 1);	
	
	uint32_t pte_base = pde;
	uint32_t pte_off = PTX(addr);
	uint32_t pte = paddr_read(pte_base + 4 * pte_off, 4);
	assert((pte&1) == 1);
	uint32_t paddr = PTE_ADDR(addr) | pte; 
	return paddr;
}

uint32_t paddr_read(paddr_t addr, int len) {
	int32_t map_no = is_mmio(addr);
	if (map_no != -1){
		return mmio_read(addr, len, map_no);
	}else{
		return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
	}
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
	int32_t map_no = is_mmio(addr);
	if (map_no != -1){
		mmio_write(addr, len, data, map_no);
		return;
	}else{
		memcpy(guest_to_host(addr), &data, len);
		return;
	}
}

uint32_t vaddr_read(vaddr_t addr, int len) {
	uint32_t addr_low = OFF(addr);
	if (addr_low <= PGSIZE && addr_low + len > PGSIZE) {
		Log("addr_low = %u\n", addr_low);
		assert(0);
	}else{
		paddr_t paddr = addr;
		if (cpu.cr0.paging == 1) paddr = page_translate(addr);
		return paddr_read(paddr, len);
	}
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
	uint32_t addr_low = OFF(addr);
	if (addr_low <= PGSIZE && addr_low + len > PGSIZE) {
		Log("addr_low = %u\n", addr_low);
		assert(0);
	}else{
		paddr_t paddr = addr;
		if (cpu.cr0.paging == 1) paddr = page_translate(addr);
		paddr_write(paddr, len, data);
	}
}

