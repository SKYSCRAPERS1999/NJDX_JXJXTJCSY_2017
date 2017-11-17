#include "common.h"
#define W 400
#define H 300
#define FD_DISPINFO 5
extern uint32_t* const fb;
extern int fs_read(int, void*, size_t);
#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
	memcpy(buf, &dispinfo[offset], len);
}

void fb_write(const void *buf, off_t offset, size_t len) {
	memcpy((uint32_t*)(fb + offset), buf, len);	
}

void init_device() {
  _ioe_init();
  char* tmp = "WIDTH:640\nHEIGHT:480\n";
	memcpy(dispinfo, tmp, strlen(tmp));
	Log("dispinfo = %s\n", dispinfo);
	//fs_read(FD_DISPINFO, dispinfo, 128);

	// TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	
}
