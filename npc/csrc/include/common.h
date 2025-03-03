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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
// #include <inttypes.h>
// #include <cinttypes>
#include <stdbool.h>
// #include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "generated/autoconf.h"
#include "include/macro.h"
#include "include/debug.h"

typedef uint32_t word_t;
typedef uint32_t  sword_t;
#define FMT_WORD "0x%08x"
typedef word_t vaddr_t;
typedef uint32_t paddr_t;
#define FMT_PADDR "0x%08x"
typedef uint16_t ioaddr_t;

//init monitor
void init_monitor(int argc, char *argv[]);
void init_sdb();
void init_log(const char *log_file);
int is_exit_status_bad();

//sdb
void sdb_mainloop();

//memory
int init_pmem();

//isa
void isa_reg_display();
word_t isa_reg_str2val(const char *s, bool *success);

enum { SIM_RUNNING, SIM_STOP, SIM_END, SIM_ABORT, SIM_QUIT };

typedef struct {
  int state;
  vaddr_t halt_pc;
  uint32_t halt_ret;
} SIMState;
extern SIMState sim_state;

// typedef struct {
//   word_t mepc;
//   word_t mstatus;
//   word_t mcause;
//   word_t mtvec;
// } CSR;

typedef struct {
  word_t gpr[MUXDEF(CONFIG_RVE, 16, 32)];
  vaddr_t pc;
  vaddr_t dnpc;
  // CSR csr;
} NPC_state;
extern NPC_state npc;


#endif
