#include <dlfcn.h>

#include <include/common.h>
#include <include/difftest/difftest.h>
#include "../memory/paddr.h"
#include "../isa/reg.h"

void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;
static void checkmem(uint8_t *ref_m, vaddr_t pc);

static bool is_skip_ref = false;
static int skip_dut_nr_inst = 0;
bool isa_difftest_checkregs(NPC_state *ref_r, vaddr_t pc);

extern uint8_t pmem[];
static uint8_t ref_pmem[CONFIG_MSIZE];

// this is used to let ref skip instructions which
// can not produce consistent behavior with NEMU
void difftest_skip_ref() {
  is_skip_ref = true;
  // If such an instruction is one of the instruction packing in QEMU
  // (see below), we end the process of catching up with QEMU's pc to
  // keep the consistent behavior in our best.
  // Note that this is still not perfect: if the packed instructions
  // already write some memory, and the incoming instruction in NEMU
  // will load that memory, we will encounter false negative. But such
  // situation is infrequent.
  skip_dut_nr_inst = 0;
}

// this is used to deal with instruction packing in QEMU.
// Sometimes letting QEMU step once will execute multiple instructions.
// We should skip checking until NEMU's pc catches up with QEMU's pc.
// The semantic is
//   Let REF run `nr_ref` instructions first.
//   We expect that DUT will catch up with REF within `nr_dut` instructions.
void difftest_skip_dut(int nr_ref, int nr_dut) {
  skip_dut_nr_inst += nr_dut;

  while (nr_ref -- > 0) {
    ref_difftest_exec(1);
  }
}

void init_difftest(char *ref_so_file, long img_size, int port) {
  assert(ref_so_file != NULL);

  void *handle;
  Log("open dl: %s", ref_so_file);
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = (void (*)(paddr_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = (void (*)(void *, bool))dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  ref_difftest_raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
  assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: %s", ANSI_FMT("ON", ANSI_FG_GREEN));
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.", ref_so_file);

  ref_difftest_init(port);
  ref_difftest_memcpy(CONFIG_MBASE, guest_to_host(CONFIG_MBASE), CONFIG_MSIZE, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&npc, DIFFTEST_TO_REF);
}

bool isa_difftest_checkregs(NPC_state *ref_r, vaddr_t pc) {
  bool check_res = false;
  for(int i = 0; i < MUXDEF(CONFIG_RVE, 16, 32); i++) {
    check_res = difftest_check_reg(reg_name(i), pc, ref_r->gpr[i], gpr(i));
    if(check_res == false) break;
  }
  return check_res;
}

bool isa_difftest_checkmem(uint8_t *ref_m, vaddr_t pc) {
  for (int i = 0; i < CONFIG_MSIZE; i++){
    if (ref_m[i] != pmem[i]) {
      // Log("[npc] after DIFFTEST_TO_DUT ref_pmem = " FMT_WORD, *(uint32_t *)(pmem + 0x80000137 - CONFIG_MBASE));
      // printf("i = %x\n", pmem + 0x80000137 - CONFIG_MBASE);
      printf(ANSI_BG_RED "memory of NPC is different before executing instruction at pc = " FMT_WORD
        ", mem[%x] right = " FMT_WORD ", wrong = " FMT_WORD ", diff = " FMT_WORD ANSI_NONE "\n",
        npc.pc, i, ref_pmem[i], pmem[i], ref_m[i] ^ pmem[i]); 
      return false;
    }
  }
  return true;
}

static void checkregs(NPC_state *ref, vaddr_t pc) {
  if (!isa_difftest_checkregs(ref, pc)) {
    sim_state.state = SIM_ABORT;
    sim_state.halt_pc = pc;
    isa_reg_display();
    // panic();
  }
}

static void checkmem(uint8_t *ref_m, vaddr_t pc) {
  if (!isa_difftest_checkmem(ref_m, pc)) {
    sim_state.state = SIM_ABORT;
    sim_state.halt_pc = pc;
  }
}

void difftest_step(vaddr_t pc, vaddr_t dnpc) {
  NPC_state ref_r;

  if (skip_dut_nr_inst > 0) {
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
    if (ref_r.pc == dnpc) {
      skip_dut_nr_inst = 0;
      checkregs(&ref_r, dnpc);
      return;
    }
    skip_dut_nr_inst --;
    if (skip_dut_nr_inst == 0)
      panic("can not catch up with ref.pc = " FMT_WORD " at pc = " FMT_WORD, ref_r.pc, pc);
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_regcpy(&npc, DIFFTEST_TO_REF);
    is_skip_ref = false;
    return;
  }

  ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
  // ref_difftest_memcpy(CONFIG_MBASE, ref_pmem, CONFIG_MSIZE, DIFFTEST_TO_DUT);

  checkregs(&ref_r, pc);
  ref_difftest_exec(1);
  // ref_difftest_memcpy(CONFIG_MBASE, ref_pmem, CONFIG_MSIZE, DIFFTEST_TO_DUT);
  // Log("[npc] after DIFFTEST_TO_DUT ref_pmem = " FMT_WORD, *(uint32_t *)(pmem + 0x80000137 - CONFIG_MBASE));
  // checkmem(ref_pmem, pc);
}


//isa difftest
void isa_difftest_attach() {
}