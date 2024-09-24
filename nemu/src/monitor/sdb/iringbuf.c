#include "sdb.h"

#define IRINGBUF_LEN 18
#define IRINGBUF_UNIT_SIZE 128

iRingbuf *rb;

void init_iringbuf() {
  rb = (iRingbuf *)malloc(sizeof(iRingbuf));
  rb->buffer = (char **)malloc(IRINGBUF_LEN * sizeof(char *));
  
  for(int i = 0; i < IRINGBUF_LEN; i++) {
    rb->buffer[i] = (char *)malloc(IRINGBUF_UNIT_SIZE * sizeof(char));
    memset(rb->buffer[i], 0, IRINGBUF_UNIT_SIZE);
  }

  rb->length = IRINGBUF_LEN;
  rb->wr_ptr = 0;
}

void free_iringbuf() {
  for (int i = 0; i < rb->length; i++) {
    free(rb->buffer[i]);
  }
  free(rb->buffer);
  free(rb);
}

void add_instruction(Decode *s) {
  char *p = rb->buffer[rb->wr_ptr];
  p += snprintf(p, IRINGBUF_UNIT_SIZE, FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for(i = ilen - 1; i >= 0; i--) {
	p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

#ifndef CONFIG_ISA_loongarch32r
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, rb->buffer[rb->wr_ptr] + IRINGBUF_UNIT_SIZE - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
#else
  p[0] = '\0'; // the upstream llvm does not support loongarch32r
#endif

  rb->wr_ptr = (rb->wr_ptr + 1) % rb->length;
}

void print_ringbuffer() {
  int i = 0;
  for(i = 0; i < IRINGBUF_LEN; i++) {
    bool current_exec_inst = ((i + 1) % IRINGBUF_LEN == rb->wr_ptr);
    if(current_exec_inst) {
      printf(" -->");
    } else {
      printf("\t");
    }
    printf("%s", rb->buffer[i]);
  }
}