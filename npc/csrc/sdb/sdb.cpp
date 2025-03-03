#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>

#include "sdb.h"
#include "include/common.h"
#include "include/sim.h"
#include "../memory/paddr.h"

static int is_batch_mode = false;

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(npc) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  npc_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
	if (sim_state.state != SIM_END) {sim_state.state = SIM_QUIT;}
  return -1;
}

static int cmd_si(char *args) {
	int args_num = (args != NULL) ? atoi(args): 1;
	npc_exec(args_num);
	return 0;
}

static int cmd_info(char *args) {
	if (args == NULL) { 
		printf("info:Missing args. \n'info r'--prints register status. \n'info w'--prints watchpoint information. \n");
		return 0;
	} 

	switch (*args) {
		case 'r': isa_reg_display(); break;
		case 'w': wp_display(); break;
		default: printf("Unknown command 'info %s'\n", args); break;
	}
	return 0;
}

static int cmd_x(char *args) {
	if (args == NULL) {
		printf("x:Missing args N. \n");
		printf("'x N EXPR'--Find the value of the expression EXPR, use the result as the starting memory address, and output N consecutive 4-bytes in hexadecimal.\n");
		return 0;
	}

	char *x_args_str_end = args + strlen(args);
	char *x_args_Num = strtok(args, " ");

	char *x_args_EXPR = x_args_Num + strlen(x_args_Num) + 1;
	if (x_args_EXPR >= x_args_str_end) {
		printf("x:Missing args EXPR. \n");
		printf("'x N EXPR'--Find the value of the expression EXPR, use the result as the starting memory address, and output N consecutive 4-bytes in hexadecimal.\n");
		return 0;
	}

	// cmd_table[6].handler(x_args_EXPR);

	word_t paddr_read(paddr_t addr, int len);
	bool expr_f = true;
	word_t mem_val;
	paddr_t args_addr = expr(x_args_EXPR, &expr_f); //temp

	if(expr_f == false) {
		Log("BAD EXPRESSION");
		return 0;
	}

	int N = atoi(x_args_Num);
	for(; N >= 1; N--){
		printf("%#x:", args_addr);
		for(int i = 3; i >= 0; i --) {
			mem_val = paddr_read(args_addr + i, 1);
			printf(" \t%#x", mem_val);
		}
		printf("\n");
		args_addr = args_addr + 4;
	}
	return 0;
}

static int cmd_p(char *args) {
	// word_t expr(char *e, bool *success);
	bool expr_f = true;
	word_t p_EXPR_res;
	p_EXPR_res = expr(args, &expr_f);
	if (expr_f != true) {
		Log("BAD EXPRESSION");
	} else {
		printf("EXPR result = %u \t %#x\n", p_EXPR_res, p_EXPR_res);
	} 	
	return 0;
}


static int cmd_w (char *args) {
	WP *w_new_wp;
	w_new_wp = new_wp();
	strcpy(w_new_wp->EXPR, args);
	bool expr_f = true;
	w_new_wp->val = expr(w_new_wp->EXPR, &expr_f);
	assert(expr_f);

	return 0;
}

static int cmd_d (char *args) {
	int wp_no = atoi(args);
	bool find_f = true;
	WP *delete_wp = find_wp(wp_no, &find_f);
	if (find_f == false) {
		printf("d:no.%d watchpoint does not exist.\n", wp_no);
		return 0;
	}
	// assert(0);
	free_wp(delete_wp);
	
	return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Lets the program pause after executing N instructions in a single step, when N is not given, the default is 1.", cmd_si },
  { "info", "info r prints register status, info w prints watchpoint information. ", cmd_info },
  { "x", "Find the value of the expression EXPR, use the result as the starting memory address, and output N consecutive 4-bytes in hexadecimal.", cmd_x },
  { "p", "Find the value of the expression EXPR", cmd_p},
  { "w", "Suspends program execution when the value of expression EXPR changes.", cmd_w},
  { "d", "Delete the monitoring point with serial number N", cmd_d}

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
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

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
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


void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();

  /* Initialize the instruction ring buffer.*/
  // init_iringbuf();
  
}