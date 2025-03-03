#include "include/sim.h"
#include "include/common.h"
#include "./memory/paddr.h"
#include "include/difftest/difftest.h"
#include "sdb/sdb.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
bool ebreak_triggered = false;
Vysyx_24070016_top* top = NULL;
NPC_state npc;
uint32_t *cpu_gpr = NULL;

#define MAX_INST_TO_PRINT 10
uint64_t g_nr_guest_inst = 0;
static bool g_print_step = false;

void ebreak_detected(svBit ebreak);
void reset_npc(int n);
void npc_exec_once();
void set_npc_state();
void set_sim_state(int state, vaddr_t pc, int halt_ret);
void set_npc_gpr_wrback();

static void trace() {
  if (g_print_step) itrace();
}

void sim_init() { 
	contextp = new VerilatedContext;
	tfp = new VerilatedVcdC;
	top = new Vysyx_24070016_top;
	contextp->traceEverOn(true);
	top->trace(tfp, 1);
	tfp->open("npc_wave.vcd");

  reset_npc(1);
}

void sim_finish() {
  if (tfp) {
    tfp->close();
    delete tfp;
    tfp = nullptr;
  }
  if (top) {
    delete top;
    top = nullptr;
  }
  if (contextp) {
    delete contextp;
    contextp = nullptr;
  }
}

void dump_wave() {
  contextp->timeInc(1);
  tfp->dump(contextp->time());
}

static void statistic() {
  Log("total guest instructions = %ld", g_nr_guest_inst);
}

void ebreak_detected(svBit ebreak) {
  if (ebreak) {
    set_sim_state(SIM_END, npc.pc, 0);
    ebreak_triggered = true;
  }
}

extern "C" void fetch_instruction(uint32_t raddr, uint32_t *rword) {
  *rword = paddr_read(raddr, 4);
  itrace_log.inst = paddr_read(raddr, 4);
}

void reset_npc(int n) {
  top->rst = 1;   // 设置复位信号为高电平

  top->clk = 0;
  top->eval();
  dump_wave();

  while(n-- > 0) {
    top->clk = 1;
    top->eval();
    dump_wave();

    top->clk = 0;
    top->eval();
    dump_wave();
  }

  set_npc_state();
  // difftest_skip_ref();
}

void npc_exec_once() {
  // 时钟上升沿：设置为 1
  top->clk = 1;
  top->eval();
  // set_npc_gpr_wrback();
  set_npc_state();
  top->rst = 0;
  top->eval();
  dump_wave();

  // 时钟下降沿：设置为 0
  top->clk = 0;
  top->eval();
  dump_wave();
}

void npc_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);

  switch (sim_state.state) {
    case SIM_END: case SIM_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: sim_state.state = SIM_RUNNING;
  }

  while(n--) {
    if(sim_state.state != SIM_RUNNING) break;
    if(ebreak_triggered) break;
    npc_exec_once();
    set_npc_state();
    // assert(0);
    // difftest_step(npc.pc, npc.dnpc);
    g_nr_guest_inst++;
    difftest_step(npc.pc, npc.dnpc);
    trace();
  }

  switch (sim_state.state) {
    case SIM_RUNNING: sim_state.state = SIM_STOP; break;

    case SIM_END: case SIM_ABORT:
      Log("npc: %s at pc = " FMT_WORD,
          (sim_state.state == SIM_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (sim_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          sim_state.halt_pc);
      // fall through
    case SIM_QUIT: statistic();
  }
}

// set cpu_gpr point to your cpu's gpr
extern "C" void set_gpr_ptr(const svOpenArrayHandle r) {
  cpu_gpr = (uint32_t *)(((VerilatedDpiOpenVar*)r)->datap());
}

void set_npc_state() {
  npc.pc = top->cur_pc;
  npc.dnpc = top->sim_dnpc;
  memcpy(&npc.gpr[0], cpu_gpr, 4 * MUXDEF(CONFIG_RVE, 16, 32));
}

void set_npc_gpr_wrback() {
  memcpy(&npc.gpr[0], cpu_gpr, 4 * MUXDEF(CONFIG_RVE, 16, 32));
}

void set_sim_state(int state, vaddr_t pc, int halt_ret) {
  // difftest_skip_ref();
  sim_state.state = state;
  sim_state.halt_pc = pc;
  sim_state.halt_ret = halt_ret;
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
  // waddr = waddr & ~0x3u;
  paddr_write(waddr, (int)wmask, wdata);
}
