#include <include/common.h>
#include "reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	int i;
	for (i = 0;i < MUXDEF(CONFIG_RVE, 16, 32);i++){
	printf("%s \t %#x \t %d \n", reg_name(i), gpr(i), gpr(i));
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	int i = 0;
	while(1){
		if (strcmp(reg_name(i), s) == 0) return gpr(i);
		i++;
	}

	*success = false;
	return 0;
}
