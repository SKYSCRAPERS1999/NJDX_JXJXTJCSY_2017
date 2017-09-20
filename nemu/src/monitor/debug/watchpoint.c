#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp()
{
	WP* p = free_;
	if (p == NULL)
	{
		assert(0);
	}else{
		free_ = free_->next;
		p->have_value = false;
		p->val_old = p->val_new = 0;
		p->next = NULL;
		return p;
	}
}

void free_wp(WP* wp)
{
	wp->next = free_;
	free_ = wp;
}
