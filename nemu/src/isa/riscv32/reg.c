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

#include <isa.h>
#include "local-include/reg.h"
#include <string.h>
#include <stdbool.h>

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	int i;
	for (i = 0;i < MUXDEF(CONFIG_RVE, 16, 32);i++){
	printf("%s \t %#x \t %d \n", reg_name(i), gpr(i), gpr(i));
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	int i = 0;
	while(1){
		if (strcmp(reg_name(i), s) == 0) return gpr(i);
		i++;
	}

	*success = false;
	return 0;
}

void isa_csr_display() {
  printf("[csr] mepc: \t %#x \t %d \n", cpu.csr.mepc, cpu.csr.mepc);
  printf("[csr] mstatus: \t %#x \t %d  \n", cpu.csr.mstatus, cpu.csr.mstatus);
  printf("[csr] mcause: \t %#x \t %d  \n", cpu.csr.mcause, cpu.csr.mcause);
  printf("[csr] mtvec: \t %#x \t %d  \n", cpu.csr.mtvec, cpu.csr.mtvec);
}

void isa_csr_write(word_t index, word_t wdata) {
  switch (index) {
    case 0x300:
      cpu.csr.mstatus = wdata; break;
    case 0x305:
      cpu.csr.mtvec = wdata; break;
    case 0x341:
      cpu.csr.mepc = wdata; break;
    case 0x342:
      cpu.csr.mcause = wdata; break;
    default:
      assert(0);
  }
}

word_t isa_csr_read(word_t index) {
  switch (index) {
    case 0x300:
      return cpu.csr.mstatus;
    case 0x305:
      return cpu.csr.mtvec;
    case 0x341:
      return cpu.csr.mepc;
    case 0x342:
      return cpu.csr.mcause;
    default:
      assert(0);
  }
}
