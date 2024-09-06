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

vluint64_t sim_cycle;
int num_inst;


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
		0x01f08113,		//addi	sp,ra,31
		0x00100073		//ebreak
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
	// printf("in pmem_read paddr = %#x\n", paddr);

	if(in_pmem(paddr)) {return host_read(guest_to_host(paddr), len);}

	printf("Memory access address out of bounds!\n");
	printf("ERROR address : %#x\n", paddr);
	assert(0);
	return 0;
}

int pmem_read_if(int pc) {
	// printf("in pmem_read_if pc = %#x\n", pc);
	return pmem_read((uint32_t)pc, 4);
}

void ebreak_detected(svBit ebreak) {
	// assert(0);
	if(ebreak) { 
		// step_and_dump_wave();

		if (tfp) {
			tfp->close();
			delete tfp;
		}
		delete top;
		delete contextp;
		// assert(0);
		printf("//////////////////////Simulation Finish//////////////////////\n");
		printf("simulation cycles = %d\n", (int)sim_cycle);
		printf("simulation instructions = %d\n", num_inst);
		exit(0); 
	}
}

int main(int argc, char** argv) {
	// nvboard_bind_all_pins(&dut);
	// nvboard_init();
	sim_init();

	sim_cycle = contextp->time();
	num_inst = init_pmem(pmem);

	top->rst = 1;   // 复位信号设为高电平
	top->clk = 0;   // 时钟低电平
	step_and_dump_wave();  // 仿真一步

	top->clk ^= 1;   // 时钟高电平，触发复位
	top->eval();
	top->inst_mem_rdata = pmem_read_if((int)top->inst_mem_addr);
	step_and_dump_wave();
	// printf("pc = %#x, inst = %#x\n", top->inst_mem_addr, top->inst_mem_rdata);
	sim_cycle++;

	while (1) {
		// if(sim_cycle == 10) break;
		top->rst = 0;   // 复位信号设为低电平
		top->clk = 0;   // 时钟低电平
		step_and_dump_wave();  // 仿真一步

		top->clk = 1;   // 时钟高电平，触发复位
		top->eval();
		top->inst_mem_rdata = pmem_read_if((int)top->inst_mem_addr);
		step_and_dump_wave();
		printf("pc = %#x, inst = %#x\n", top->inst_mem_addr, top->inst_mem_rdata);
		sim_cycle++;
	}

	// int half_clock_period = 2; //1周期10个步长
	// top->clk = 1;
	// top->rst = 1;
	// printf("contextp->time() = %d", (int)contextp->time());

	// while(1) {
	// 	if(contextp->time() % (2*half_clock_period) == 0) sim_cycle++;

	// 	if(contextp->time() % half_clock_period == 0) top->clk ^= 1;
	// 	if(contextp->time() >= (2*half_clock_period)) top->rst = 0;
	// 	step_and_dump_wave();
		
	// 	if(top->clk == 1) top->inst_mem_rdata = pmem_read_if((int)top->inst_mem_addr);
	// 	// printf("pc = %#x, inst = %#x\n", top->inst_mem_addr, top->inst_mem_rdata);
	// }
	
	// nvboard_quit();
	if (tfp) {
		tfp->close();
		delete tfp;
	}
	delete top;
	delete contextp;
}