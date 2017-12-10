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
		
	return 0;
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
		assert(0);
	}else{
		//paddr_t paddr = page_translate(addr);
		return paddr_read(addr, len);
	}
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  paddr_write(addr, len, data);
}

