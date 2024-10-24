#include <isa.h>
#include "sdb.h"

#define STT_FUNC 2

Elf32_Sym *symtab_buf = NULL;
int symtab_entrynum;
char *strtab_buf = NULL;

ftrace_event ft = {NO_CALL, 0};
ftrace_event *ftrace_monitor = &ft;

char *ftrace_log_buf = NULL;
int len_log_buf = 0;

char *read_elf_file(char *elf_file) {
  FILE *fp = fopen(elf_file, "r");
  if(fp == NULL) {
    printf("fail to open the elf file.\n");
    assert(0);
  }
  fseek(fp, 0, SEEK_END);
  int elf_size = ftell(fp);
  char *elf_buf = (char *)malloc(elf_size);
  
  int a;
  rewind(fp);
  a = fread(elf_buf, elf_size, 1, fp);
  if(a == 0) {
    printf("fail to read elf file\n");
    assert(0);
  }

  a = fclose(fp);
  if(a != 0) {
    printf("fail to close elf file\n");
    assert(0);
  }

  return elf_buf;
}

void init_elf(char *elf_file) {
  char *elf_buf = read_elf_file(elf_file);

  Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_buf;
  // printf("\n//%d//\n", ehdr.e_shstrndx);

  if((ehdr->e_ident[0] != 0x7F) ||
    ehdr->e_ident[1] != 'E' ||
    ehdr->e_ident[2] != 'L' ||
    ehdr->e_ident[3] != 'F') {
    printf("the file is not ELF.\n");
    assert(0);
  }

  Elf32_Shdr *shdr = (Elf32_Shdr *)(elf_buf + ehdr->e_shoff);
  Elf32_Shdr *shdr_shstrtab = shdr + ehdr->e_shstrndx;
  char *shstrtab = elf_buf + shdr_shstrtab->sh_offset;
  
  int i;
  Elf32_Shdr *shdr_strtab = NULL;
  Elf32_Shdr *shdr_symtab = NULL;
  for(i = 0; i < ehdr->e_shnum; i++) {
    Elf32_Shdr *this_shdr = shdr + i;
    char *section_name = shstrtab + this_shdr->sh_name;
    if(strcmp(section_name, ".strtab") == 0) {
      shdr_strtab = this_shdr;
    } else if(strcmp(section_name, ".symtab") == 0) {
      shdr_symtab = this_shdr;
    }
    // printf("////%s////\n", section_name);
  }

  symtab_buf = (Elf32_Sym *)malloc(shdr_symtab->sh_size);
  memcpy(symtab_buf, elf_buf + shdr_symtab->sh_offset, shdr_symtab->sh_size); 
  symtab_entrynum = shdr_symtab->sh_size / shdr_symtab->sh_entsize;

  strtab_buf = (char *)malloc(shdr_strtab->sh_size);
  memcpy(strtab_buf, elf_buf + shdr_strtab->sh_offset, shdr_strtab->sh_size);

  // printf("\nst num = %d \n", shdr_symtab->sh_size / shdr_symtab->sh_entsize);
  // printf("\nfunc name = %s\n", strtab_buf + (symtab_buf + )->st_name);
  // assert(0);

  free(elf_buf);
}

bool is_function_call(int is_jal, int is_jalr, int rd) {
  if((is_jal == 1) && (rd == 1 || rd == 5)) {
    return true;
  }
  if((is_jalr == 1) && (rd == 1 || rd == 5)) {
    return true;
  }
  return false;
}

bool is_function_ret(int is_jal, int is_jalr, int rd, word_t src1) {
  assert(0);
  if((is_jalr == 1) && (rd == 0) && (src1 == 1 || src1 == 5)) {
    return true;
  }
  return false;
}

void ftrace_identify_call_ret(int is_jal, int is_jalr, int rd, word_t src1) {
  if(is_function_call(is_jal, is_jalr, rd)) {
    ftrace_monitor->state = FUNC_CALL;
  } else if(is_function_ret(is_jal, is_jalr, rd, src1)) {
    assert(0);
    ftrace_monitor->state = FUNC_RET;
  }
}

void new_ftrace_log(vaddr_t pc, int call_depth, int is_call, char *func_name, vaddr_t dnpc) {
  int len_pc = ((sizeof(vaddr_t) * 8) / 4) + 4; //"0xxxxxxxx: " 12
  int len_call_depth = (call_depth - 1) * 2;
  int len_func_type = 5;                        //"call " or "ret  " 4
  int len_jump_target = len_pc + 1 + strlen(func_name);
  int new_log = len_pc + len_call_depth + len_func_type + len_jump_target + 1; //+1 "\n"
  printf("\npc:"FMT_WORD "dnpc:"FMT_WORD "\n", pc, dnpc);
  printf("pc=%d, calldep=%d, functype=%d, jump=%d, new=%d\n", len_pc, len_call_depth, len_func_type, len_jump_target, new_log);

  //alloc one ftrace message
  ftrace_log_buf = realloc(ftrace_log_buf, len_log_buf + new_log + 1);
  if (ftrace_log_buf == NULL) {
  perror("realloc failed\n");
  exit(1);
  }
  char *cur_log_position = ftrace_log_buf + len_log_buf;
  len_log_buf = len_log_buf + new_log;
  
  sprintf(cur_log_position, FMT_WORD ": ", pc);
  printf("pc = %s\n", cur_log_position);
  cur_log_position += len_pc;
  memset(cur_log_position, ' ', len_call_depth);
  cur_log_position += len_call_depth;
  if(is_call) {
    sprintf(cur_log_position, "call ");
  } else {
    sprintf(cur_log_position, "ret  ");
  }
  cur_log_position += len_func_type;
  printf("func_name = %s, len_func_name = %d\n", func_name, (int)strlen(func_name));
  printf("len_cur_log = %d, len_ft_log = %d, \nlog:%s|\n", new_log, len_log_buf, ftrace_log_buf + len_log_buf - new_log);

  // assert(0);
  sprintf(cur_log_position, "[%s@" FMT_WORD "]\n", func_name, dnpc);
  printf("log:%s\n", ftrace_log_buf);
  
}

void ftrace_run_onece(vaddr_t pc, vaddr_t dnpc) {
  int call_depth = ftrace_monitor->call_depth;
  int is_call = ftrace_monitor->state == FUNC_CALL;
  ftrace_monitor->call_depth = is_call ? 
                                ++call_depth : --call_depth;

  int i;
  char *target_func_name = "???";
  for (i = 0; i < symtab_entrynum; i++) {
    Elf32_Sym *cur_symtab = &symtab_buf[i];
    //STT_FUNC: 2
    if((ELF32_ST_TYPE(cur_symtab->st_info) == STT_FUNC)
        && (dnpc >= cur_symtab->st_value
        && dnpc < (cur_symtab->st_value + cur_symtab->st_size))) {
        target_func_name = strtab_buf + (symtab_buf + i)->st_name;
    }
  }
  new_ftrace_log(pc, ftrace_monitor->call_depth, is_call, target_func_name, dnpc);

}
void free_ftrace() {
  free(symtab_buf);
  free(strtab_buf);
  free(ftrace_log_buf);
}

void ftrace_log_print() {
  log_write("ftrace message: \n%s\n", ftrace_log_buf);
  free_ftrace();
}


