#include "fs.h"
extern void ramdisk_read(void*, off_t, size_t);
extern void ramdisk_write(const void*, off_t, size_t);
typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
	//
	off_t open_offset;
	//
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char* pathname, int flags, int mode){
	int i = FD_NORMAL;
	for (i = FD_NORMAL; i < NR_FILES; i++){
		if (strcmp(pathname, file_table[i].name) == 0){
			break;
		}
	}
	assert(i < NR_FILES);
	return i;
}

int fs_read(int fd, void* buf, size_t len){
	ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	return len;
}

int fs_write(int fd, void* buf, size_t len){
	ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	return len;
}

int fs_close(int fd){
	return 0;
}
