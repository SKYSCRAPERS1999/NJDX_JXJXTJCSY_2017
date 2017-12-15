#include "common.h"
//HERE HAS BEEN CHANGED
#define DEFAULT_ENTRY ((void *)0x8048000)
void ramdisk_read(void *buf, off_t offset, size_t len);
size_t get_ramdisk_size();
extern int fs_filesz(int);
extern int fs_open(const char*, int, int);
extern int fs_read(int, void*, size_t);
extern int fs_close(int);
extern bool fs_end(int);
extern void _map(_Protect *, void* , void*);
extern void* new_page(void);
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
	//ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
	Log("ENTER!\n");
	int fd = fs_open(filename, 0, 0);
  int N = (fs_filesz(fd) + PGSIZE - 1) / PGSIZE;
	for (int i = 0; i <= N; i++){
		void* ENTRY = new_page();
		_map(as, (void*)(DEFAULT_ENTRY + i * PGSIZE), ENTRY);
		Log("ENTRY[%d] = 0x%x\n",i, (uint32_t)ENTRY);
		fs_read(fd, ENTRY, PGSIZE);
	}
	fs_close(fd);
	Log("%s loaded\n", filename);
	return (uintptr_t)DEFAULT_ENTRY;
}
