#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  char *orign_dst = dst;
  while(*src != '\0') {
	*dst = *src;
	dst++;
	src++;
  }
  *dst = '\0';
  return orign_dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  char *orign_dst = dst;
  while (*dst != '\0') {
	dst++;
  }
  while(*src != '\0') {
	*dst = *src;
	dst++;
	src++;
  }
  *dst = '\0';
  return orign_dst;
}

int strcmp(const char *s1, const char *s2) {
  while((*s1 != '\0') && (*s2 != '\0') && (*s1 == *s2)) {
	s1++;
	s2++;
  }
  const unsigned char ch1 = *s1;
  const unsigned char ch2 = *s2;

  return ch1 - ch2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  int i;
  unsigned char ch = c;
  unsigned char *dst = s;
  for(i = 0; i < 0; i++) {
    *dst = ch;
    dst++;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  int i;
  const unsigned char *ch1 = s1;
  const unsigned char *ch2 = s2;
  for(i = 0; i < n; i++) {
	if(*ch1 == *ch2) {
	  ch1++;
	  ch2++;
	  continue;
	}
	return *ch1 - *ch2;
  }
  return 0;
}

#endif
