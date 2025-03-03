#ifndef __SIM_H__
#define __SIM_H__

// #include <verilated.h>
#include <verilated_dpi.h>
#include "Vysyx_24070016_top.h"
#include "verilated_vcd_c.h"
#include "Vysyx_24070016_top__Dpi.h"

extern Vysyx_24070016_top* top;
extern bool ebreak_triggered;
extern uint32_t *cpu_gpr;

void sim_init();
void sim_finish();

void npc_exec(uint64_t n);

#endif
