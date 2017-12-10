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

uint32_t page_translate(vaddr_t addr, bool is_write) {
	uint32_t pde_base = PTE_ADDR(cpu.cr3);
	uint32_t pde_off = PDX(addr);
	uint32_t pde = paddr_read(pde_base + 4 * pde_off, 4);
	//Log("pde_base = %x\n", pde_base);
	//Log("addr = %x\n", addr);
	//Log("pde_off = %x\n", pde_off);
	//Log("pde = %x\n", pde);
	assert((pde&1) == 1);	
	
	uint32_t pte_base = PTE_ADDR(pde);
	uint32_t pte_off = PTX(addr);
	uint32_t pte = paddr_read(pte_base + 4 * pte_off, 4);
	assert((pte&1) == 1);
	paddr_t paddr = PTE_ADDR(pte) | OFF(addr); 
	
	paddr_write(pde_base + 4 * pde_off, 4, pde | 0x20);	
	paddr_write(pte_base + 4 * pte_off, 4, pte | 0x20);	
	if (is_write) paddr_write(pte_base + 4 * pte_off, 4, pte | 0x60);
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

uint32_t cross_pg_read(vaddr_t addr, int len){
	uint32_t n1 = PGSIZE - OFF(addr);
	uint32_t n2 = OFF(addr + len - 1);
	assert(OFF(addr) + n1 == PGSIZE);
	assert(addr + len == PGSIZE + n2);
	uint32_t data;
	uint8_t *data_array = (uint8_t*)&data;
	int p = 0;
	for (int i = 0; i < n1; i++) {
		data_array[p++] = paddr_read(addr + i, 1);
	}
	for (int i = 0; i < n2; i++) {
		data_array[p++] = paddr_read(addr + n1 + i, 1);
	}
	return data;
}

void cross_pg_write(vaddr_t addr, int len, uint32_t data){
	uint8_t *data_array = (uint8_t*)&data;	
	for (int i = 0; i < len; i++) {
		paddr_write(page_translate(addr + i, true), 1, data_array[i]);
	}
}

uint32_t vaddr_read(vaddr_t addr, int len) {
	if (((cpu.cr0 & 0x1) == 0) || ((cpu.cr0 & 0x80000000) == 0)) return paddr_read(addr, len);
	uint32_t ok = PTE_ADDR((addr)^(addr + len - 1));
	if (ok != 0) {
		//Log("addr = %u\n", addr);
		return cross_pg_read(addr, len);
		//assert(0);
	}else{
	  paddr_t paddr = page_translate(addr, false);
		return paddr_read(paddr, len);
	}
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
	if (((cpu.cr0 & 0x1) == 0) || ((cpu.cr0 & 0x80000000) == 0)) {
		paddr_write(addr, len, data);
		return;
	}
	uint32_t ok = PTE_ADDR((addr)^(addr + len - 1));
	if (ok != 0) {
		//Log("addr = %u\n", addr);
		cross_pg_write(addr, len, data);
		//assert(0);
	}else{
		paddr_t paddr = page_translate(addr, true);
		paddr_write(paddr, len, data);
	}
}

