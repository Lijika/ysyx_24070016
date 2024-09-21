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