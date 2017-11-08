#include "cpu/exec.h"
void raise_intr(uint8_t, vaddr_t);
void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
	print_asm_template1(lidt);
	
	t1 = vaddr_read(id_dest->addr, 2);
	cpu.IDT_lim = t1;
	t2 = vaddr_read(id_dest->addr + 2, 4);
	if (id_dest->width == 2){
		t2 = t2 & 0x00ffffff;
	}
	cpu.IDT_base = t2;
	Log("cpu.IDT_base = 0x%x", cpu.IDT_base);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
	raise_intr(id_dest->val, decoding.seq_eip);
	rtl_pop(&cpu.eip);
	//decoding.is_jmp = 0;
	//rtl_pop(&decoding.seq_eip);
	//cpu.eip = decoding.seq_eip;
	//rtl_pop(&cpu.EFLAGS);
	Log("ENTERED\n");
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  TODO();

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
	t1 = pio_read(id_src->val, id_dest->width);
	operand_write(id_dest, &t1);
  
	print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
	pio_write(id_dest->val, id_src->width, id_src->val);

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
