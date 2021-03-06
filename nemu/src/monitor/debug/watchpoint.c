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
		return 0;
		//assert(0);
	}else{
		free_ = free_->next;
		strcpy(p->express, args);
		p->val_old = 0; 
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

bool free_wp(int pos)
{
	WP *p = head;
	WP *q = head;
	while (p != NULL)
	{
		if (p->NO == pos)
		{
			break;
		}
		q = p;
		p = p->next;
	}
  if (p != NULL)
	{
		if (p == head)
		{
			head = head->next;
			p->next = free_;
			free_ = p;
		}else{
			q->next = p->next;
			p->next = free_;
			free_ = p;
		}
		return 1;
	}else{
		//printf("Not, found!\n");
		return 0;
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
	bool success = true;
	uint32_t new_value = 0;
	while (p != NULL)
	{
		success = true;
	  new_value = expr(p->express, &success, 'n');
		if (!success)
		{
			Log("expr error");
			return 0;
			//assert(0);
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
				bool success = true;
				uint32_t new_exp = expr(p->express, &success, 'n');
				if (!success) {
					Log("expression error\n");
					return;
					//assert(0);
				}
				p->changed = false;
				uint32_t val_old = p->val_old;
				p->val_old = new_exp;
				printf("%d\t%s\t\t0x%x\t0x%x\t\n", p->NO, p->express, val_old, p->val_old);
			}
			p = p->next;
		}
	}
}
