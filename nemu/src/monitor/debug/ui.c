#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

// static int cmd_x_cnt = 1;

void cpu_exec(uint64_t);

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
		sscanf(arg1, "%d", &N);
		sscanf(arg2, "0x%x", &EXPR);
		assert(N >= 0);
		for (uint32_t i = 0; i < N; i++){
		    if (i % 4 == 0) {if (i) {printf("\n");} printf("0x%08x:", EXPR + i);}
			uint8_t DEST = vaddr_read(EXPR + i, 1);
			printf("%02x ", DEST);
		}
		printf("\n");
	}
	return 0;
}

static int cmd_p(char *args)
{
	printf("%s\n", args);
	bool success = false;
	expr(args, &success);
	return success;
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
  { "info", "Get info, use r to acquire registers", cmd_info}, 
  { "x", "Scan memory", cmd_x},
  { "p", "Print expression", cmd_p},
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
		sscanf(arg, "%d", &n);
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



