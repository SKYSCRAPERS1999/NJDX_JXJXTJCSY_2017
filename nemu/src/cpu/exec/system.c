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
	//Log("cpu.IDT_base = 0x%x", cpu.IDT_base);
}

make_EHelper(mov_r2cr) {
  //TODO();
	if (id_dest->reg == 0) {
		cpu.cr0.val = id_src->val;
		Log("cr0 = %x\n", cpu.cr0.val);
	}else if (id_dest->reg == 3){
		cpu.cr3.val = id_src->val; 
		Log("cr3 = %x\n", cpu.cr3.val);
	}else{assert(0);}
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
	if (id_src->reg == 0) {
		operand_write(id_dest, &cpu.cr0.val);
		}else if (id_src->reg == 3){
		operand_write(id_dest, &cpu.cr3.val);
	}else{assert(0);}
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
	raise_intr(id_dest->val, decoding.seq_eip);
	//Log("ENTERED\n");
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
	rtl_pop(&t1);
	decoding.seq_eip = cpu.eip = t1;
	decoding.is_jmp = 0;
	rtl_pop(&t1);
	cpu.cs = (uint16_t)t1;
	rtl_pop(&t1);
	cpu.eflags = t1;
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
