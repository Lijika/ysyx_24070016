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

#ifndef __ISA_H__
#define __ISA_H__

// Located at src/isa/$(GUEST_ISA)/include/isa-def.h
#include <isa-def.h>

// The macro `__GUEST_ISA__` is defined in $(CFLAGS).
// It will be expanded as "x86" or "mips32" ...
typedef concat(__GUEST_ISA__, _CPU_state) CPU_state;
typedef concat(__GUEST_ISA__, _ISADecodeInfo) ISADecodeInfo;

// monitor
extern unsigned char isa_logo[];
void init_isa();

// reg
extern CPU_state cpu;
void isa_reg_display();
word_t isa_reg_str2val(const char *name, bool *success);
void isa_csr_display();

// exec
struct Decode;
int isa_exec_once(struct Decode *s);

// memory
enum { MMU_DIRECT, MMU_TRANSLATE, MMU_FAIL };
enum { MEM_TYPE_IFETCH, MEM_TYPE_READ, MEM_TYPE_WRITE };
enum { MEM_RET_OK, MEM_RET_FAIL, MEM_RET_CROSS_PAGE };
#ifndef isa_mmu_check
int isa_mmu_check(vaddr_t vaddr, int len, int type);
#endif
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type);

// interrupt/exception
vaddr_t isa_raise_intr(word_t NO, vaddr_t epc);
#define INTR_EMPTY ((word_t)-1)
word_t isa_query_intr();

// difftest
bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc);
void isa_difftest_attach();

#endif

/*

uint32_t *cpu_gpr = NULL;
uint32_t *cpu_mstatus = NULL;
uint32_t *cpu_mtvec = NULL;
uint32_t *cpu_mepc = NULL;
uint32_t *cpu_mcause = NULL;

extern "C" void fetch_instruction(uint32_t raddr, uint32_t *rword) {
  *rword = paddr_read(raddr, 4);
#ifdef CONFIG_ITRACE
  itrace_log.inst = paddr_read(raddr, 4);
#endif
}

void set_sim_state(int state, vaddr_t pc, int halt_ret) {
  // difftest_skip_ref();
  sim_state.state = state;
  sim_state.halt_pc = pc;
  sim_state.halt_ret = halt_ret;
}

extern "C" void ebreak_detected(svBit ebreak) {
  if (ebreak) {
    set_sim_state(SIM_END, npc.pc, 0);
    ebreak_triggered = true;
  }
}

// set cpu_gpr point to your cpu's gpr
extern "C" void set_gpr_ptr(const svOpenArrayHandle r) {
  cpu_gpr = (uint32_t *)(((VerilatedDpiOpenVar*)r)->datap());
}

extern "C" void set_csr_ptr(const svOpenArrayHandle mstatus, const svOpenArrayHandle mtvec, const svOpenArrayHandle mepc, const svOpenArrayHandle mcause) {
  cpu_mstatus = (uint32_t *)(((VerilatedDpiOpenVar*)mstatus)->datap());
  cpu_mtvec = (uint32_t *)(((VerilatedDpiOpenVar*)mtvec)->datap());
  cpu_mepc = (uint32_t *)(((VerilatedDpiOpenVar*)mepc)->datap());
  cpu_mcause = (uint32_t *)(((VerilatedDpiOpenVar*)mcause)->datap());
}

void set_npc_state() {
  //pc + reg
  npc.pc = top->cur_pc;
  npc.dnpc = top->sim_dnpc;
  // memcpy(&npc.gpr[0], cpu_gpr, sizeof(npc.gpr));
  //csr
  // memcpy(&npc.csr.mstatus, cpu_mstatus, sizeof(word_t));
  // memcpy(&npc.csr.mtvec, cpu_mtvec, sizeof(word_t));
  // memcpy(&npc.csr.mepc, cpu_mepc, sizeof(word_t));
  // memcpy(&npc.csr.mcause, cpu_mcause, sizeof(word_t));
}

extern "C" int npc_datamem_read(int raddr, char rmask) {
  // 总是读取地址为`raddr & ~0x3u`的4字节返回
  // raddr = raddr & ~0x3u;

  return paddr_read(raddr, (int)rmask);
}

extern "C" void npc_datapmem_write(int waddr, int wdata, char wmask) {
  // 总是往地址为`waddr & ~0x3u`的4字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
  
  paddr_write(waddr, (int)wmask, wdata);
}
*/