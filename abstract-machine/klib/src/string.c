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

  return *(unsigned char*)s1 - *(unsigned char*)s2;
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
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  for(size_t i = 0; i < n; i++) {
	if(p1[i] != p2[i]) {
	  return p1[i] - p2[i];
	}
  }
  return 0;
}

#endif
