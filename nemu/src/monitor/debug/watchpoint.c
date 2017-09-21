#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

WP* get_head(){ return head;}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
		wp_pool[i].val_old = -1;
		wp_pool[i].changed = false;
	}
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char *args)
{
	WP* p = free_;
	if (p == NULL)
	{
		Log("Not enough pool!\n");
		assert(0);
	}else{
		free_ = free_->next;
		
		strcpy(p->express, args);
		bool success = false;
		//Log("expression = %s\n", p->express);
		p->val_old = expr(p->express, &success, 'n');
		if (!success) 
		{
		  Log("expression failed!\n");
			assert(0);
		}
	  
		if (head != NULL)
		{
			WP *q = head;
			while (q->next != NULL)
			{
				q = q->next;
			}
			q->next = p;
			p->next = NULL;
		}else{
			head = p;
			p->next = NULL;
		}
		return head;
	}
}

void free_wp(WP* wp)
{
	WP *p = head;
	WP *q = head;
	while (p != NULL)
	{
		if (p == wp)
		{
			break;
		}
		q = p;
		p = p->next;
	}
  if (p != NULL)
	{
		q->next = wp->next;
		wp->next = free_;
		free_ = wp;
	}else{
		Log("Not, found!\n");
		assert(0);
	}
}

//WP* add_wp(char* args)
//{
//	WP* p = new_wp();
//	p->express = args;
//	bool success = 0;
//	p->val_old = expr(args, &success,  'x');
//	p->next = head->next;
//	head->next = p;
//	return p;
//}

bool check_wp()
{
	bool flag = false;
	WP *p = head;
	bool success = false;
	uint32_t new_value = 0;
	while (p != NULL)
	{
		success = false;
	  new_value = expr(p->express, &success, 'n');
		if (!success)
		{
			Log("expr error");
			assert(0);
		}else{
			if (new_value != p->val_old)
			{
				flag = true;
				p->changed = true;
			}
		}
		p = p->next;
	}
	return flag;
}

void dis_wp()
{
	if (head == NULL)
	{
		printf("No watchpoints!\n");
	}else{
		WP *p = head;
		printf("No.\tExpr\t\tval_old\tval_new\t\n");
    while (p != NULL)
		{
			if (p->changed)
			{
				p->changed = false;
				uint32_t val_old = p->val_old;
				bool success = false;
				p->val_old = expr(p->express, &success, 'n' );
				if (!success) {
					Log("expr error\n");
					assert(0);
				}
				printf("%d\t%s\t\t0x%x\t0x%x\t\n", p->NO, p->express, val_old, p->val_old);
			}
			p = p->next;
		}
	}
}
