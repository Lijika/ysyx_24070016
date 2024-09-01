#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <verilated.h>
#include "Vysyx_24070016_top.h"
#include "verilated_vcd_c.h"
// #include <nvboard.h>

#define PMEMSIZE 0x8000000
#define PMEMBASE 0x80000000

// static TOP_NAME dut;
// void nvboard_bind_all_pins(Vtop* top);

uint8_t pmem[PMEMSIZE] __attribute((aligned(4096))) = {};

void init_pmem(uint8_t *pmem) {
	const uint32_t img [] = {
		0x01000093,		//li	ra,16
		0x01008113,		//addi	sp,ra,16
		0x01f08113		//addi	sp,ra,31
	};

	memcpy(pmem, img, sizeof(img));
}

uint32_t pmem_read(uint32_t paddr, int len) {
	bool in_pmem = paddr - PMEMBASE < PMEMSIZE;
	uint8_t* host_addr = paddr - PMEMBASE + pmem;
	
	if(in_pmem) return *(uint32_t *)host_addr;

	printf("Memory access address out of bounds!");
	assert(0);
}

int main(int argc, char** argv) {
	// nvboard_bind_all_pins(&dut);
	// nvboard_init();

	VerilatedContext* contextp = new VerilatedContext;
	contextp->commandArgs(argc,argv);
	Vysyx_24070016_top* top = new Vysyx_24070016_top{contextp};

	Verilated::traceEverOn(true);
	VerilatedVcdC* tfp = new VerilatedVcdC;
	top->trace(tfp, 1);
	tfp->open("wave.fst");
	vluint64_t sim_time = contextp->time();
	
	init_pmem(pmem);


	while (1) {
		if (sim_time >= 3) {
			break;
		}

		top->clk = 0;
		top->rst = 0;
		top->
		top->eval();
		contextp->timeInc(1);

		top->clk = 1;
		top->eval();
		contextp->timeInc(1);
	}

	// nvboard_quit();
	if (tfp) {
		tfp->close();
		delete tfp;
	}
	delete top;
	delete contextp;
}