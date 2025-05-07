#include <include/common.h>
#include <include/memory/host.h>
#include <include/memory/paddr.h>
#include <include/device/mmio.h>

static inline uint32_t inst_fetch(vaddr_t pc, int len);
static word_t pmem_read(paddr_t addr, int len);
static void pmem_write(paddr_t addr, int len, word_t data);
static inline word_t host_read(void *addr, int len);
static inline void host_write(void *addr, int len, word_t data);

uint8_t pmem[CONFIG_MSIZE] PG_ALIGN = {};

uint8_t* guest_to_host(paddr_t paddr) { return pmem + paddr - CONFIG_MBASE; }
paddr_t host_to_guest(uint8_t *haddr) { return haddr - pmem + CONFIG_MBASE; }

static void out_of_bound(paddr_t addr) {
  panic("address = " FMT_PADDR " is out of bound of pmem [" FMT_PADDR ", " FMT_PADDR "] at pc = " FMT_WORD,
      addr, PMEM_LEFT, PMEM_RIGHT, npc.pc);
}

static inline uint32_t inst_fetch(vaddr_t pc, int len) {
  uint32_t inst = paddr_read(pc, len);
  return inst;
}

static word_t pmem_read(paddr_t addr, int len) {
  word_t ret = host_read(guest_to_host(addr), len);
  return ret;
}

static void pmem_write(paddr_t addr, int len, word_t data) {
  host_write(guest_to_host(addr), len, data);
}

word_t paddr_read(paddr_t addr, int len) {
  // if(m->is_access_mem) update_mtrace_buffer(addr, len, 1);
  if (likely(in_pmem(addr))) return pmem_read(addr, len);
  IFDEF(CONFIG_DEVICE, return mmio_read(addr, len));
  out_of_bound(addr);
  return 0;
}

void paddr_write(paddr_t addr, int len, word_t data) {
  // if(m->is_access_mem) update_mtrace_buffer(addr, len, 0);
  if (likely(in_pmem(addr))) { pmem_write(addr, len, data); return; }
  IFDEF(CONFIG_DEVICE, mmio_write(addr, len, data); return);
  out_of_bound(addr);
}

int init_pmem() {
	const uint32_t img [] = {
    //addi指令测试：
		0x01000093,		//li	ra,16
    0xfbb00793,
    0x00f0c193,   //xori x3, x1, 15
    0xff200113,   //li  sp,-14
    // 0x00209863,   //beq ra sp ebreak

    // 0xfff0a193,     //slti x3, x1, -1
    // 0xfff12193,    //slti x3, x2, -1   1
    // 0x0010b193,    //sltiu x3, x1, 1
    // 0x0140b193,   //sltiu x3, x1, 20   1
		// 0x01008113,		//addi	sp,ra,16
		// 0x01f08113,		//addi	sp,ra,31
    // 0x001101b3,   //add x3, x2, x1
		0x00100073		//ebreak



    // //dummy程序指令测试：
    // 0x00000093,  // addi x1, x0, 0        // li x1, 0
    // 0x00008193,  // addi x2, x1, 1        // x2 = x1 + 1, x2 = 0 + 1 = 1
    // 0x00010193,  // addi x3, x2, 2        // x3 = x2 + 2, x3 = 1 + 2 = 3
    // 0x00000097,  // auipc x4, 0           // x4 = PC + 0 (auipc instruction sets x4 to PC)
    // 0x00000113,  // addi x6, x0, 0        // x6 = 0 (to prepare for jump target)
    // 0x00004037,  // lui x7, 0x00400000    // x7 = 0x00400000
    // 0x0040006f,  // jal x0, 0x10          // jump to 0x10 (PC + 0x10)
    // 0x00000067,  // jalr x0, 0(x6)        // jump to address in x6 (0), program ends here
		// 0x00100073		//ebreak
	};

	memcpy(pmem, img, sizeof(img));
	return sizeof(img) / sizeof(uint32_t);
}