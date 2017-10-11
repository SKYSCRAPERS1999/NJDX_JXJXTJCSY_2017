#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
#define I8042_DATA_PORT 0x60
static unsigned long boot_time;
void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  //return 0;
	boot_time ++;
	outl(RTC_PORT, boot_time);
	return boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int i;
  for (i = 0; i < _screen.width * _screen.height; i++) {
    //uint32_t yy = i / _screen.width;
		//uint32_t xx = i % _screen.width;
		//if (xx >= x && xx <= x + w && yy >= y && yy <= y + h){
			//fb[i] = pixels[i];
			memcpy(fb + i, pixels + i, 4);
		//}
  }
}

void _draw_sync() {
}

int _read_key() {
	uint32_t code = inl(I8042_DATA_PORT);
	return code;
  //return _KEY_NONE;
}
