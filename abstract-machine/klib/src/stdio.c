#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int int2str(int src, char *str) {
  int is_neg = 0;
  int i = 0;
  if(src < 0) {
    src *= -1;
    is_neg = 1;
  }

  while(src) {
    str[i++] = (src % 10) + '\0';
    src /= 10;
  }

  if(is_neg) { str[i++] = '-'; }
  str[i] = '\0';

  int lchar = 0;
  int rchar = i - 1;
  while(lchar < rchar) {
    char temp = str[lchar];
    str[lchar] = str[rchar];
    str[rchar] = temp;
    lchar++;
    rchar--;
  }
  return i;
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  int ch_num = 0;
  va_list ap;
  
  va_start(ap, fmt);
  while(*fmt != '\0') {
    if(*fmt != '%') {
      *(out++) = *(fmt++);
      ch_num++;
      continue;
    }
    
    switch(*(fmt+1)) {
      case 's': {
        char *str = va_arg(ap, char*);
        while(*str) { *(out++) = *(str++); ch_num++; }
        break;
      }
      case 'd': {
        int num = va_arg(ap, int);
        int d_ch_num = int2str(num, out);
        out += d_ch_num;
        ch_num += d_ch_num;
        break;
      }
      default: {
        *(out++) = *fmt;
        ch_num++;
        break;
      }
    }
    fmt += 2;
  }
  va_end(ap);

  return ch_num;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
