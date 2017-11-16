#include "common.h"
#include "syscall.h"
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
		case SYS_none: {
			SYSCALL_ARG1(r) = 1;	
			break;
		}
		case SYS_write: {
			SYSCALL_ARG1(r) = SYS_write;
			break;
		}
		case SYS_exit: {
			SYSCALL_ARG1(r) = SYS_exit;
			_halt(SYSCALL_ARG2(r));
		}
						
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
