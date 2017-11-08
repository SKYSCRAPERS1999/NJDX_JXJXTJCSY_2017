#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //TODO();
	ret_addr = cpu.eip;
	rtl_push(&cpu.EFLAGS);
	rtl_push(&ret_addr);
	uint32_t base = cpu.IDT_base;
	uint32_t idt_data[2];
	idt_data[0] = vaddr_read(base + 8 * NO, 4);
	idt_data[1] = vaddr_read(base + 8 * NO + 4, 4);
	uint32_t idta = (idt_data[0] & 0xffff) | (idt_data[1] & 0xffff0000);
	cpu.eip = idta;
}

void dev_raise_intr() {
}
