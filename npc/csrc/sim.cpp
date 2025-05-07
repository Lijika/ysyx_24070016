#include <include/sim.h>
#include <include/common.h>
#include <include/difftest/difftest.h>
#include <include/memory/paddr.h>
#include "sdb/sdb.h"

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;
bool ebreak_triggered = false;
Vysyx_24070016_top* top = NULL;

#define MAX_INST_TO_PRINT 10
uint64_t g_nr_guest_inst = 0;
static bool g_print_step = false;

void ebreak_detected(svBit ebreak);
void reset_npc(int n);
void npc_exec_once();

void device_update();

static void trace() {
#ifdef CONFIG_ITRACE
  if (g_print_step) itrace();
#endif

#ifdef CONFIG_DIFFTEST
  difftest_step(npc.pc, npc.dnpc);  //difftest 
#endif
}

void sim_init() { 
	contextp = new VerilatedContext;
	top = new Vysyx_24070016_top;

#ifdef CONFIG_WAVETRACE
  contextp->traceEverOn(true);
  tfp = new VerilatedVcdC;
	top->trace(tfp, 1);
	tfp->open("npc_wave.vcd");
#endif

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
#ifdef CONFIG_WAVETRACE
  contextp->timeInc(1);
  tfp->dump(contextp->time());
#endif
}

static void statistic() {
  Log("total guest instructions = %ld", g_nr_guest_inst);
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
  // difftest_step(npc.pc, npc.dnpc);  //difftest 
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
    npc_exec_once();
    // assert(npc.pc != 0x800011fc);

    set_npc_state();
    g_nr_guest_inst++;
    trace();
    if(sim_state.state != SIM_RUNNING) break;
    if(ebreak_triggered) break;
    IFDEF(CONFIG_DEVICE, device_update());
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

void assert_fail_msg() {
  #ifdef CONFIG_ITRACE
    // print_ringbuffer();
    // free_iringbuf();
  #endif
    isa_reg_display();
    statistic();
  }