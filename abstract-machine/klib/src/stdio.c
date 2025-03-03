#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int int2str(int num, char *str) {
  char buffer[16];  // 临时缓冲区（int 最大长度约 11 位）
  int i = 0;
  int is_neg = 0;

  if (num == 0) {
    str[0] = '0';
    return 1;
  }

  // 处理负数（使用无符号类型避免溢出）
  unsigned int unum;
  if (num < 0) {
    is_neg = 1;
    unum = (unsigned int)(-num);  // 安全转换（避免溢出）
  } else {
    unum = (unsigned int)num;
  }

  // 写入数字（逆序）
  while (unum > 0) {
    buffer[i++] = (unum % 10) + '0';
    unum /= 10;
  }

  // 添加负号
  if (is_neg) {
    buffer[i++] = '-';
  }

  // 反转字符串
  int len = i;
  for (int j = 0; j < len; j++) {
    str[j] = buffer[len - j - 1];
  }

  return len;
}

int printf(const char *fmt, ...) {
  // panic("Not implemented");

  char buffer[32768]; 
  va_list args;
  
  va_start(args, fmt);
  int len = vsprintf(buffer, fmt, args); 
  va_end(args);
  
  putstr(buffer);  // 使用你的 putstr 输出
  return len;      // 返回生成的字符数
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int ch_num = 0;
  while (*fmt != '\0') {
    if (*fmt != '%') {
      *(out++) = *(fmt++);
      ch_num++;
      continue;
    }

    fmt++;  // 跳过 '%'

    // 解析格式修饰符（例如 %02d）
    int width = 0;
    int zero_pad = 0;

    // 处理填充和宽度（如 %02d 中的 02）
    if (*fmt == '0') {
      zero_pad = 1;
      fmt++;
    }
    while (*fmt >= '0' && *fmt <= '9') {
      width = width * 10 + (*fmt - '0');
      fmt++;
    }

    // 解析格式符（如 d）
    switch (*fmt) {
      case 's': {
        char *str = va_arg(ap, char*);
        while (*str != '\0') {
            *(out++) = *str++;
            ch_num++;
        }
        fmt++;
        break;
      }
      case 'd': {
        int num = va_arg(ap, int);
        char num_buffer[16];
        int num_len = int2str(num, num_buffer);

        // 处理宽度和填充
        int padding = width - num_len;
        if (padding > 0) {
            for (int i = 0; i < padding; i++) {
                *(out++) = zero_pad ? '0' : ' ';
                ch_num++;
            }
        }

        // 写入数字
        for (int i = 0; i < num_len; i++) {
            *(out++) = num_buffer[i];
            ch_num++;
        }
        fmt++;
        break;
      }
      case 'c': {
        int c = va_arg(ap, int);
        *(out++) = (char)c;
        ch_num++;
        fmt++;
        break;
      }
      default: {
        // 非法格式符（如 %% 或 %x）
        *(out++) = '%';
        *(out++) = *fmt;
        ch_num += 2;
        fmt++;
        break;
      }
    }
  }
  *out = '\0';
  return ch_num;
}

int sprintf(char *out, const char *fmt, ...) {
  // panic("Not implemented");

  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
