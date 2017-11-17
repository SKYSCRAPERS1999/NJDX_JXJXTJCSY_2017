#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
void ramdisk_read(void *buf, off_t offset, size_t len);
size_t get_ramdisk_size();
extern int fs_filesz(int);
extern int fs_open(const char*, int, int);
extern int fs_read(int, void*, size_t);
extern int fs_close(int);
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
	//ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  //return (uintptr_t)DEFAULT_ENTRY;
	
	int fd = fs_open(filename, 0, 0);
	fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
	fs_close(fd);
	return (uintptr_t)DEFAULT_ENTRY;
}
