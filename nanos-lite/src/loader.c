#include "common.h"
//HERE HAS BEEN CHANGED
#define DEFAULT_ENTRY ((void *)0x8048000)
void ramdisk_read(void *buf, off_t offset, size_t len);
size_t get_ramdisk_size();
extern int fs_filesz(int);
extern int fs_open(const char*, int, int);
extern int fs_read(int, void*, size_t);
extern int fs_close(int);
extern void _map(_Protect *, void* , void*);
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
	//ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  //return (uintptr_t)DEFAULT_ENTRY;
	void* ENTRY = NULL;
	_map(as, DEFAULT_ENTRY, ENTRY);
	Log("ENTRY = 0x%x\n", (uint32_t)ENTRY);
	Log("%s loaded\n", filename);	
	int fd = fs_open(filename, 0, 0);
	fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
	fs_close(fd);
	return (uintptr_t)ENTRY;
	//return (uintptr_t)DEFAULT_ENTRY;
}
