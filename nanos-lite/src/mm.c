#include "proc.h"
#include "memory.h"
void _map(_Protect*, void*, void*);
static void *pf = NULL;

void* new_page(void) {
  assert(pf < (void *)_heap.end);
  void *p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
	Log("current->cur_brk = 0x%x\n", current->cur_brk);
	Log("current->max_brk = 0x%x\n", current->max_brk);
	Log("new_brk = 0x%x\n", new_brk);
	if (current->cur_brk == 0){
		current->cur_brk = current->max_brk = new_brk;
	}else{
		if (new_brk > current->max_brk){
			int N = (new_brk - current->max_brk + PGSIZE - 1)	/ PGSIZE;
			for (int i = 0; i < N; i++) { 
				void* pa = new_page();
				_map(current->as.ptr, (void*)(current->max_brk + i * PGSIZE), pa);
			}
			current->max_brk = new_brk;
		}

		current->cur_brk = new_brk;
	}

  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
}
