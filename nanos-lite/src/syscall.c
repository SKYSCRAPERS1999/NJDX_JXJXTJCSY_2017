#include "common.h"
#include "syscall.h"
extern intptr_t end, _end;
extern int fs_open(const char*, int, int);
extern int fs_read(int, void*, size_t);
extern int fs_close(int);
extern int fs_lseek(int, off_t, int);
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
			Log("Exited : status = %d\n", a[1]);
			_halt(a[1]);
			break;
		}
		
		case SYS_brk: {
			_heap.end = (void*)a[1];
			Log("_heap.end = %p\n, _end = %p, end = %p\n", _heap.end, &_end, &end);
			SYSCALL_ARG1(r) = 0;
			break;
		}
		
		case SYS_open: {
			const char* path = (const char*)a[1];
			SYSCALL_ARG1(r) = fs_open(path, 0, 0);
			Log("Opened\n");
			break;
		}
		
		case SYS_read: {
			SYSCALL_ARG1(r) = fs_read(a[1], (void*)(a[2]), a[3]);
			Log("Readed\n");
			break;
		}

		case SYS_close: {
			SYSCALL_ARG1(r) = fs_close(a[1]);
			Log("Closed\n");
			break;
		}

		case SYS_lseek:{
			SYSCALL_ARG1(r) = fs_lseek(a[1], a[2], a[3]);
			Log("Lseeked\n");
			break;
		}
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
