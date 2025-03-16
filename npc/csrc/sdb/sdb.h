
#ifndef __SDB_H__
#define __SDB_H__

#include <include/common.h>

//init sdb
void init_regex();
void init_wp_pool();

//sdb
void sdb_set_batch_mode();

word_t expr(char *e, bool *success);
// typedef struct watchpoint WP;
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  bool en;
//   int addr;
  word_t val;
  int hit_count;
  char EXPR[32*32];
} WP;

WP* new_wp();
WP* find_wp (int wp_no, bool *success);
void free_wp(WP *wp);
void scanf_wp_head (bool *hit);
void wp_display ();

typedef struct Itrace_log {
  uint32_t inst;
  char logbuf[128];
} Itrace_log;
extern Itrace_log itrace_log;

void itrace();

#endif
