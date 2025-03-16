#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
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
  char *orign_dst = dst;
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dst[i] = src[i];
  }
  memset(dst + i, '\0', n - i);
  return orign_dst;
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
  for (size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
      return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *dst = (unsigned char *)s;
  for(size_t i = 0; i < n; i++) {
    dst[i] = (unsigned char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;
  if (d != s && (d > s + n || s > d + n)) {
    return memcpy(dst, src, n);
  } else {
    for (size_t i = n; i > 0; i--) {
      d[i - 1] = s[i - 1];
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  unsigned char *d = (unsigned char *)out;
  const unsigned char *s = (const unsigned char *)in;
  for (size_t i = 0; i < n; i++) {
    d[i] = s[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *ch1 = (const unsigned char *)s1;
  const unsigned char *ch2 = (const unsigned char *)s2;
  for(size_t i = 0; i < n; i++) {
	if(ch1[i] != ch2[i]) {
	  return ch1[i] - ch2[i];
	}
  }
  return 0;
}

#endif
