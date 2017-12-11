#include "fs.h"
#define W 400
#define H 300
extern void _ioe_init();
extern void ramdisk_read(void*, off_t, size_t);
extern void ramdisk_write(const void*, off_t, size_t);
extern void dispinfo_read(void*, off_t, size_t);
extern void fb_write(const void*, off_t, size_t);
extern size_t events_read(void*, size_t);
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
	//file_table[FD_FB].size = W * H * sizeof(uint32_t);	
	file_table[FD_FB].size = _screen.width * _screen.height * sizeof(uint32_t);	
	
}

int fs_open(const char* pathname, int flags, int mode){
	int i = 0;
	for (i = 0; i < NR_FILES; i++){
		if (strcmp(pathname, file_table[i].name) == 0){
			break;
		}
	}
	assert(i < NR_FILES);
	file_table[i].open_offset = 0;
	return i;
}

int fs_read(int fd, void* buf, size_t len){
	assert(file_table[fd].open_offset <= file_table[fd].size);
	if (file_table[fd].open_offset + len > file_table[fd].size){
			len = file_table[fd].size - file_table[fd].open_offset;
	}
	switch(fd){
		case FD_STDIN: 
		case FD_STDOUT:
		case FD_STDERR: 
		case FD_FB: break;
		case FD_EVENTS:{
			len = events_read(buf, len);
			file_table[FD_EVENTS].open_offset += len;
			break;
		}
		case FD_DISPINFO :{
			dispinfo_read(buf, file_table[FD_DISPINFO].open_offset, len);
			//Log("buf = %s\n", buf);
			file_table[FD_DISPINFO].open_offset += len;
			break;
		}
		default:{
			//if (file_table[fd].open_offset + len > file_table[fd].size){
			//	len = file_table[fd].size - file_table[fd].open_offset;
			//}
			ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
		}
	}
	return len;
}

int fs_write(int fd, void* buf, size_t len){
	if (file_table[fd].open_offset + len > file_table[fd].size){
			len = file_table[fd].size - file_table[fd].open_offset;
	}
	switch(fd){
		case FD_STDOUT:
		case FD_STDERR:
		case FD_EVENTS:
		case FD_DISPINFO: break;
		case FD_FB: {
			fb_write(buf, file_table[FD_FB].open_offset, len);
			file_table[FD_FB].open_offset += len;
			break;
		}
		default: {
			//if (file_table[fd].open_offset + len > file_table[fd].size){
			//	len = file_table[fd].size - file_table[fd].open_offset;
			//}
			ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
			file_table[fd].open_offset += len;
		}
	}
	return len;
}

int fs_close(int fd){
	return 0;
}

int fs_filesz(int fd){
	return file_table[fd].size;
}

off_t fs_lseek(int fd, off_t offset, int whence){
	switch(whence){
		case SEEK_SET:{
			if (offset > file_table[fd].size) { offset = file_table[fd].size;}
			file_table[fd].open_offset = offset;
			break;
		}
		case SEEK_CUR:{
			if (offset + file_table[fd].open_offset > file_table[fd].size) {offset = file_table[fd].size - file_table[fd].open_offset;}
			file_table[fd].open_offset += offset;
			break;
		}
		case SEEK_END:{
			file_table[fd].open_offset = file_table[fd].size + offset;
			break;
		}
		default: assert(0);
	}
	return file_table[fd].open_offset;
}


