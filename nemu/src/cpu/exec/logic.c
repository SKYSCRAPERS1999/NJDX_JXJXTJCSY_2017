#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
	rtl_and(&t2, &id_dest->val, &id_src->val);
  
	rtl_update_ZFSF(&t2, id_dest->width);
	
	t0 = 0;
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
  
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
	//Log("dest = %d ## src = %d\n", id_dest->val, id_src->val);
	rtl_and(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
		
  rtl_update_ZFSF(&t2, id_dest->width);
			
	t0 = 0;
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
	rtl_xor(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
		
  rtl_update_ZFSF(&t2, id_dest->width);
			
	t0 = 0;
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);
	
	print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
	rtl_or(&t2, &id_dest->val, &id_src->val);
	operand_write(id_dest, &t2);
		
  rtl_update_ZFSF(&t2, id_dest->width);
			
	t0 = 0;
	rtl_set_CF(&t0);
	rtl_set_OF(&t0);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
	t1 = id_dest->val;
	rtl_sar(&t2, &t1, &id_src->val);
	id_dest->val = (int32_t)t2;
	operand_write(id_dest, &id_dest->val);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  
	print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
	t1 = id_dest->val;
	//t1 <<= id_src->val;
	rtl_shl(&t2, &t1, &id_src->val);
	id_dest->val = (uint32_t)t2;
	//operand_write(id_dest, &t2);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);
  
	print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
	t1 = id_dest->val;
	rtl_shr(&t2, &t1, &id_src->val);
	//t1 >>= id_src->val;
	operand_write(id_dest, &t2);
	rtl_update_ZFSF(&id_dest->val, id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
	t1 = id_dest->val;
	rtl_not(&t1);
	operand_write(id_dest, &t1);
  print_asm_template1(not);
}
