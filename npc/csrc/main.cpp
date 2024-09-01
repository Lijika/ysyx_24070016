#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <verilated.h>
#include "Vysyx_24070016_top.h"
#include "verilated_vcd_c.h"
// #include <nvboard.h>

// static TOP_NAME dut;

// void nvboard_bind_all_pins(Vtop* top);

int main(int argc, char** argv) {
	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc,argv);
	Vysyx_24070016_top* top = new Vysyx_24070016_top{contextp};

	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	top->trace(tfp, 1);
	tfp->open("wave.fst");

	// nvboard_bind_all_pins(&dut);
	// nvboard_init();
	
	vluint64_t sim_time = contextp->time();

	while (!contextp->gotFinish())
	{

	}

	// nvboard_quit();
	if (tfp) {
		tfp->close();
		delete tfp;
	}
	delete top;
	delete contextp;
}