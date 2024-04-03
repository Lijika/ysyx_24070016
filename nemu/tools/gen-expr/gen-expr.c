/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose (uint32_t n) {
	return rand() % n;
}

int buf_pt;
int has_divided;
int max_gen_expr_depth;
int buf_overflow;

static void enbuf (char *s) {
	if (buf_overflow == 1) return;

	int gen_len;
	gen_len = strlen(s);
	int i;
	for (i = 0; i < gen_len; i++) {
		
		buf[buf_pt + i] = s[i];
	}
	buf_pt = buf_pt + gen_len;

	if (buf_pt == 65536 - 1) buf_overflow = 1;
}

static void gen_num() {
	int num_int = rand() % (2 ^ (sizeof(int) * 8 / 2));
	char num_char[20];

	if (has_divided) num_int++;		//avoid generate zero

	sprintf(num_char, "%d", num_int);
	enbuf(num_char);
}

static void gen_tk(char tk) {
	char tk_t[2];
	tk_t[0] = tk;
	tk_t[1] = '\0';
	enbuf(tk_t);
} 

static void gen_rand_op() {
	switch (choose(4)) {
		case 0: gen_tk('+'); break;
		case 1: gen_tk('-'); break;
		case 2: gen_tk('*'); break;
		case 3: 
			gen_tk('/'); 
			has_divided = 1;
			gen_num();
			break;
	}
}

static void gen_rand_expr() {
	// max_gen_expr_depth--;
	// if (max_gen_expr_depth == 0) {
	// 	gen_num();
	// 	max_gen_expr_depth++;
	// 	return;
	// }

	//
	if (buf_overflow == 1) return;

	switch (choose(3)) {
		case 0: gen_num(); break;
		case 1: gen_tk('('); gen_rand_expr(); gen_tk(')'); break;
		default: 
			gen_rand_expr(); 
			gen_rand_op(); 
			if (has_divided == 0) { gen_rand_expr(); }
			has_divided = 0;
			break;
	}
	if (choose(3) == 0) gen_tk(' ');

	// max_gen_expr_depth++;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
	has_divided = 0;
	buf_pt = 0;
	// max_gen_expr_depth = 15;
	while (1) {
		memset(buf, '\0', sizeof(buf));
		buf_overflow = 0;
		gen_rand_expr();
		if (buf_overflow == 0) { break; }
	}

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
