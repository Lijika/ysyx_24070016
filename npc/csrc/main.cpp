#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <verilated.h>
#include "Vtop.h"
#include "verilated_vcd_c.h"
#include <nvboard.h>

static TOP_NAME dut;

void nvboard_bind_all_pins(Vtop* top);

int main(int argc, char** argv) {
	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc,argv);
	Vtop* top = new Vtop{contextp};

	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	top->trace(tfp, 0);
	tfp->open("wave.vcd");

	nvboard_bind_all_pins(&dut);
	nvboard_init();
	

	while (!contextp->gotFinish())
	{
		int a = rand() & 1;
		int b = rand() & 1;
		top->a = a;
		top->b = b;

		top->eval();
		tfp->dump(contextp->time());
		contextp->timeInc(1);
		printf("a = %d, b = %d, f = %d\n", a, b, top->f);
		assert(top->f == (a ^ b));
		while (1) 
		{
			dut.eval();
			nvboard_update();
		}
	}

	nvboard_quit();
	tfp->close();
}