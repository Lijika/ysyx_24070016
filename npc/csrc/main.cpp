#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <verilated.h>
#include "Vysyx_24070016_top.h"
#include "verilated_vcd_c.h"
#include "svdpi.h"
#include "Vysyx_24070016_top__Dpi.h"
// #include <nvboard.h>

#define PMEMSIZE 0x8000000
#define PMEMBASE 0x80000000

// static TOP_NAME dut;
// void nvboard_bind_all_pins(Vtop* top);

VerilatedContext* contextp = NULL;
VerilatedVcdC* tfp = NULL;

static Vysyx_24070016_top* top;

void sim_init(){
	contextp = new VerilatedContext;
	tfp = new VerilatedVcdC;
	top = new Vysyx_24070016_top;
	contextp->traceEverOn(true);
	top->trace(tfp, 1);
	tfp->open("dump.vcd");
}

void step_and_dump_wave(){
	top->eval();
	contextp->timeInc(1);
	tfp->dump(contextp->time());
}

uint8_t pmem[PMEMSIZE] __attribute((aligned(4096))) = {};

int init_pmem(uint8_t *pmem) {
	const uint32_t img [] = {
		0x01000093,		//li	ra,16
		0x01008113,		//addi	sp,ra,16
		0x01f08113		//addi	sp,ra,31
	};

	memcpy(pmem, img, sizeof(img));
	return sizeof(img) / sizeof(uint32_t);
}

bool in_pmem(uint32_t paddr) {
	return paddr - PMEMBASE < PMEMSIZE;
}

uint8_t* guest_to_host(uint32_t paddr) { return pmem + paddr - PMEMBASE; }
uint32_t host_to_guest(uint8_t *haddr) { return haddr - pmem + PMEMBASE; }

uint32_t host_read(void *addr, int len) {
	switch (len) {
		case 1: return *(uint8_t  *)addr;
		case 2: return *(uint16_t *)addr;
		case 4: return *(uint32_t *)addr;
		default: assert(0);
	}
}

uint32_t pmem_read(uint32_t paddr, int len) {
	if(in_pmem(paddr)) {return host_read(guest_to_host(paddr), len);}

	printf("Memory access address out of bounds!\n");
	printf("ERROR address : %#x\n", paddr);
	assert(0);
	// return 0;
}

int pmem_read_if(int pc) {
	return pmem_read((uint32_t)pc, 4);
}

int main(int argc, char** argv) {
	// nvboard_bind_all_pins(&dut);
	// nvboard_init();
	sim_init();

	vluint64_t sim_cycle = contextp->time();
	int num_inst = 3;

	while (1) {
		if (sim_cycle > num_inst) {
			break;
		}

		if (sim_cycle == 0) {
			top->rst = 1;top->clk = 0;
			step_and_dump_wave();

			top->rst = 1;top->clk = 1;
			step_and_dump_wave();
		}

		top->rst = 0;

		top->clk = 0;
		step_and_dump_wave();
		top->clk = 1;
		step_and_dump_wave();
		sim_cycle++;
	}

	// nvboard_quit();
	if (tfp) {
		tfp->close();
		delete tfp;
	}
	delete top;
	delete contextp;
}