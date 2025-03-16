#include <include/sim.h>
#include <include/common.h>
#include <include/memory/paddr.h>
#include "sdb/sdb.h"
#include <include/difftest/difftest.h>

NPC_state npc = {};

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
  memcpy(&npc.gpr[0], cpu_gpr, sizeof(npc.gpr));
  //csr
  memcpy(&npc.csr.mstatus, cpu_mstatus, sizeof(word_t));
  memcpy(&npc.csr.mtvec, cpu_mtvec, sizeof(word_t));
  memcpy(&npc.csr.mepc, cpu_mepc, sizeof(word_t));
  memcpy(&npc.csr.mcause, cpu_mcause, sizeof(word_t));
}

extern "C" void detecte_device_addr(int addr) {
  if(!in_pmem(addr)) {difftest_skip_ref();}
}

extern "C" int npc_datamem_read(int raddr, char rmask) {
  // 总是读取地址为`raddr & ~0x3u`的4字节返回
  // raddr = raddr & ~0x3u;
  // assert(npc.pc != 0x800011fc);
  // if(!in_pmem(raddr)) {
  //   printf(" rmem pc = "FMT_WORD"\n", npc.pc);
  //   difftest_skip_ref();
  // }

  return paddr_read(raddr, (int)rmask);
}

extern "C" void npc_datapmem_write(int waddr, int wdata, char wmask) {
  // 总是往地址为`waddr & ~0x3u`的4字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变

  paddr_write(waddr, (int)wmask, wdata);
}