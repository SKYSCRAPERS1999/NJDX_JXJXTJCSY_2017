#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

uint32_t cmd_p_cnt = 0;
void cpu_exec(uint64_t);
WP* new_wp(char*);
WP* get_head();
bool free_wp(int);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_info(char *args)
{	
  char *arg = strtok(NULL, " ");
	if (arg != NULL && strcmp(arg, "r") == 0)
	{
		char a[8][4] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};
		for (uint32_t i = R_EAX; i <= R_EDI; i++)
		{ 
			printf("%s : 0x%x\n", a[i], reg_l(i)); 
		}
		printf("EIP : 0x%x\n", cpu.eip);
		printf("ZF : 0x%x\n", cpu.ZF);
		printf("SF : 0x%x\n", cpu.SF);
		printf("CF : 0x%x\n", cpu.CF);
		printf("OF : 0x%x\n", cpu.OF);
	}else if (arg != NULL && strcmp(arg, "w") == 0){
		WP* p = get_head();
		if (p == NULL)
		{
			printf("No watchpoints!\n");
			return 0;
		}
		printf("No.\tExpr\t\tval\t\n");
		while (p != NULL)
		{
			printf("%d\t%s\t\t0x%x\t\n", p->NO, p->express, p->val_old);
			p = p->next;
		}
	}
	return 0;
}

static int cmd_d(char *args)
{
  int pos = atoi(args);
	printf("pos = %d\n", pos);
	bool success = free_wp(pos);	
	if (success)
	{
		printf("Watchpoint at pos %d deleted\n", pos);
	}else{	
		printf("Not, found!\n");
	}
	return 0;
}

static int cmd_x(char *args)
{ 
  char *arg1 = strtok(NULL, " ");
	char *arg2 = strtok(NULL, " ");
	uint32_t EXPR; int N;
	//printf("%s\n", arg1);
	//printf("%s\n", arg2);
	if (arg1 != NULL && arg2 != NULL)
	{
    if (strlen(arg1) >= 2 && arg1[0] == '0' && arg1[1] == 'x') { sscanf(arg1, "0x%x", &N);}
    else {sscanf(arg1, "%d", &N);}

    //if (strlen(arg2) >= 2 && arg2[0] == '0' && arg2[1] == 'x') { sscanf(arg2, "0x%x", &EXPR);}
    //else {sscanf(arg2, "%d", &EXPR);}
		//sscanf(arg2, "0x%x", &EXPR);
		assert(N >= 0);
		bool success = false;
		EXPR = expr(arg2, &success, 'n');
		for (uint32_t i = 0; i < N; i++){
		    if (i % 4 == 0) {if (i) {printf("\n");} printf("0x%08x:", EXPR + i);}
			uint8_t DEST = vaddr_read(EXPR + i, 1);
			printf("%02x ", DEST);
		}
		printf("\n");
	}
	return 0;
}

// three types of cmd_p
static int cmd_p(char *args)
{
	bool success = false;
	cmd_p_cnt++;
	expr(args, &success, 'd');
	return success;
}

static int cmd_px(char *args)
{
	bool success = false;
	cmd_p_cnt++;
	expr(args, &success, 'x');
	return success;
}
static int cmd_pd(char *args)
{
	bool success = false;
	cmd_p_cnt++;
	expr(args, &success, 'd');
	return success;
}

static int cmd_w(char *args)
{
	//Log("args = %s\n", args);
	new_wp(args);
  return 0;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Take i steps", cmd_si},
  { "info", "Get info, use r to acquire registers, w to acquire watchpoints", cmd_info}, 
  { "x", "Scan memory", cmd_x},
  { "p", "Print expression", cmd_p},
  { "p/x", "Print expression (hex)", cmd_px},
  { "p/d", "Print expression (decimal)", cmd_pd},
  { "w", "Create watchpoint", cmd_w	},
	{ "d", "Delete watchpoint", cmd_d},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char* args)
{
  char *arg = strtok(NULL, " ");
	int n = 1;
	if (arg == NULL){
		cpu_exec(n);
	}
	else {
		if (strlen(arg) >= 2 && arg[0] == '0' && arg[1] == 'x') { sscanf(arg, "0x%x", &n);}
		else { sscanf(arg, "%d", &n);}
		cpu_exec(n);
	}
	return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}



