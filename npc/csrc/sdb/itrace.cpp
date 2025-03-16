#include <include/common.h>
#include "sdb.h"

#ifdef CONFIG_ITRACE
Itrace_log itrace_log;

static void itrace_once(Itrace_log *s) {
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", npc.pc);
  int ilen = 4;//riscv32指令长度为4字节
  int i;
  uint8_t *inst = (uint8_t *)&s->inst;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;
}

void itrace() {
  itrace_once(&itrace_log);
  puts(itrace_log.logbuf);
}

#endif