#include "common.h"
_RegSet* schedule(_RegSet*);

_RegSet* do_syscall(_RegSet*);
static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
		case 8:{
			return do_syscall(r);
		}
		
		case 7:{
			//Log("Hello!\n");
			return schedule(r);	
		}
	
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
