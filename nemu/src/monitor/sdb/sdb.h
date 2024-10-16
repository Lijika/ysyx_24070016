/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>
#include <cpu/decode.h>
#include <elf.h>

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

typedef struct iRingbuf {
  char **buffer;
  int length;
  int wr_ptr;
} iRingbuf;

extern iRingbuf *rb;
void free_iringbuf();
void add_instruction(Decode *s);
void print_ringbuffer();

#ifdef CONFIG_MTRACE
typedef struct log_mtrace_buffer {
  char log[128];
  bool is_access_mem;
} log_mtrace_buffer;
extern log_mtrace_buffer *m;
#endif

WP* new_wp();
WP* find_wp (int wp_no, bool *success);
void free_wp(WP *wp);
void scanf_wp_head (bool *hit);
void wp_display ();

//ftrace
#define FTRACE_IS_FUNC_CALL 1
#define FTRACE_IS_FUNC_RET 2
extern Elf32_Sym *symtab_buf;
extern char *strtab_buf;

typedef enum {
    NO_CALL,    // 既不是函数调用，也不是函数返回
    FUNC_CALL,  // 函数调用
    FUNC_RET    // 函数返回
} ftrace_state;
typedef struct {
  ftrace_state state;
  int call_depth;
} ftrace_event;

extern ftrace_event *ftrace_monitor;
void ftrace_run_onece(vaddr_t pc, vaddr_t dnpc);



#endif
