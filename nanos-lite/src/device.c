#include "common.h"
#define W 400
#define H 300
#define FD_DISPINFO 5
extern uint32_t* fb;
extern int fs_read(int, void*, size_t);
extern int _read_key();
extern unsigned long _uptime();
#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
	int code = _read_key();
	//printf("code = %d\n", code);
	if (code != _KEY_NONE){
		if (code > 0x8000){
			sprintf(buf, "kd %s\n", keyname[code - 0x8000]);
			
		}else{
			sprintf(buf, "ku %s\n", keyname[code]);
		}
	}
	else{
		unsigned long time = _uptime();
		sprintf(buf, "t %d\n", time);
	}
	return strlen(buf);
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
	memcpy(buf, (char*)(dispinfo + offset), len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
	memcpy((uint32_t*)(fb + offset / 4), buf, len);	
}

void init_device() {
  _ioe_init();
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", _screen.width, _screen.height);

	// TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	
}
