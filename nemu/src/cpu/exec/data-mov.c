#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
	rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
	rtl_pop(&t0);
	operand_write(id_dest, &t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
	if (id_dest->width == 2)
	{
		rtl_push((uint32_t*)&reg_w(R_EAX));
		rtl_push((uint32_t*)&reg_w(R_ECX));
		rtl_push((uint32_t*)&reg_w(R_EDX));
		rtl_push((uint32_t*)&reg_w(R_EBX));
		t1 = reg_w(R_ESP);
		rtl_push(&t1);
		t1 = reg_w(R_EBP);
		rtl_push(&t1);
		t1 = reg_w(R_ESI);
		rtl_push(&t1);
		t1 = reg_w(R_EDI);
		rtl_push(&t1);
	}else if (id_dest->width == 4){
		rtl_push(&reg_l(R_EAX));
		rtl_push(&reg_l(R_ECX));
		rtl_push(&reg_l(R_EDX));
		rtl_push(&reg_l(R_EBX));
		t1 = reg_l(R_ESP);
		rtl_push(&t1);
		t1 = reg_l(R_EBP);
		rtl_push(&t1);
		t1 = reg_l(R_ESI);
		rtl_push(&t1);
		t1 = reg_l(R_EDI);
		rtl_push(&t1);
	}
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();

	if (id_dest->width == 2)
	{
		rtl_pop(&t1);
		rtl_sr(R_EDI, 2, &t1);
		rtl_pop(&t1);
		rtl_sr(R_ESI, 2, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EBP, 2, &t1);
		rtl_pop(&t1);
		//rtl_sr(R_ESP, 2, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EBX, 2, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EDX, 2, &t1);
		rtl_pop(&t1);
		rtl_sr(R_ECX, 2, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EAX, 2, &t1);
	}else if (id_dest->width == 4){
		rtl_pop(&t1);
		rtl_sr(R_EDI, 4, &t1);
		rtl_pop(&t1);
		rtl_sr(R_ESI, 4, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EBP, 4, &t1);
		rtl_pop(&t1);
		//rtl_sr(R_ESP, 4, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EBX, 4, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EDX, 4, &t1);
		rtl_pop(&t1);
		rtl_sr(R_ECX, 4, &t1);
		rtl_pop(&t1);
		rtl_sr(R_EAX, 4, &t1);
	}
  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
	rtl_mv(&cpu.esp, &cpu.ebp);
	rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
		t0 = 0;
		rtl_lr(&t1, R_AX, 2);
		rtl_msb(&t0, &t1, 2);
		if(t0){
			t2 = 0xffff;
		}else{
			t2 = 0;
		}
		rtl_sr(R_DX, 2, &t2);
  }
  else {
    //TODO();
		t0 = 0;
		rtl_lr(&t1, R_EAX, 4);
		rtl_msb(&t0, &t1, 4);
		if(t0){
			t2 = 0xffffffff;
		}else{
			t2 = 0;
		}
		rtl_sr(R_EDX, 4, &t2);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
		t0 = 0;
		rtl_lr(&t1, R_AX, 2);
		rtl_msb(&t0, &t1, 2);
		if(t0){
			t2 = 0xffff;
		}else{
			t2 = 0;
		}
		rtl_sr(R_AH, 2, &t2);
	}else {
		//TODO();
		rtl_lr(&t1, R_AX, 2);
		rtl_sext(&t2, &t1, 2);
		rtl_sr(R_EAX, 4, &t2);
	}

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
