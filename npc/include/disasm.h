#ifndef __DISASM_H__
#define __DISASM_H__

#include <iostream>
// #include "common.h"

#ifdef CONFIG_ITRACE
extern "C" void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
extern "C" void init_disasm(const char *triple);
#else
void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte) {}
void init_disasm(const char *triple) {}
#endif

#endif
