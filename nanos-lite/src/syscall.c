#include "common.h"
#include "syscall.h"
extern _Area _heap;
extern intptr_t end, _end;
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
	a[1] = SYSCALL_ARG2(r);
	a[2] = SYSCALL_ARG3(r);
	a[3] = SYSCALL_ARG4(r);
  switch (a[0]) {
		case SYS_none: {
			SYSCALL_ARG1(r) = 1;	
			break;
		}
		
		case SYS_write: {
			int fd = a[1];
			char *buf = (char*)a[2];
			size_t count = a[3];
			int i = 0;
			if (fd == 1 || fd == 2){
				for (i = 0; i < count; i++){
					_putc(buf[i]);
				}
			}
			SYSCALL_ARG1(r) = count;
			Log("_heap.end = %p\n", _heap.end);
			break;
		}

		case SYS_exit: {
			SYSCALL_ARG1(r) = SYS_exit;
			Log("status = %d\n", a[1]);
			_halt(a[1]);
			break;
		}
		
		case SYS_brk: {
			//_heap.end = (void*)SYSCALL_ARG2(r);
			Log("_heap.end = %p\n, _end = %p, end = %p\n", _heap.end, &_end, &end);
			SYSCALL_ARG1(r) = 0;
			break;
		}
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
