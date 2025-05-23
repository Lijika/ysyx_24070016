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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include <stdbool.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_INT_NUM, TK_HEX_NUM, TK_NEQ, TK_AND, TK_DEREF, TK_REG

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},			//minus
  {"\\*", '*'},			//multiply
  {"\\/", '/'},			//divided
  {"\\(", '('},
  {"\\)", ')'},
  {"0x[0-9a-z]+", TK_HEX_NUM},
  {"[0-9]+", TK_INT_NUM},


  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"&&", TK_AND},
  {"\\*", TK_DEREF},		//The make_token() function will not match this character 
  {"\\$(\\$0|([0-9a-z]+))", TK_REG},

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

		if(substr_len >= 32) {
			printf("EXPR: tokens.str overflow.\n");
			return false;
		}

        switch (rules[i].token_type) {
		  case TK_NOTYPE: break;
		  
          default:
		  	tokens[nr_token].type = rules[i].token_type;
			strncpy(tokens[nr_token].str, substr_start, substr_len);
			tokens[nr_token].str[substr_len] = '\0';
			printf("tokens[%d].str = %s \n", nr_token, tokens[nr_token].str);
		  	nr_token++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

word_t eval(int p, int q);

word_t expr(char *e, bool *success) {
	if (!make_token(e)) {
		*success = false;
		return 0;
	}

//   assert(0);
  /* TODO: Insert codes to evaluate the expression. */
	int i;
	for (i = 0; i < nr_token; i ++) {
		if (tokens[i].type == '*' && 
			(i == 0 || tokens[i - 1].type == '+'
					|| tokens[i - 1].type == '-'
					|| tokens[i - 1].type == '*'
					|| tokens[i - 1].type == '/'
					|| tokens[i - 1].type == '('
					|| tokens[i - 1].type == TK_EQ
					|| tokens[i - 1].type == TK_NEQ
					|| tokens[i - 1].type == TK_AND) ) {
			tokens[i].type = TK_DEREF;
		}
	}

	return eval(0, nr_token - 1);
}

int check_parentheses(int p, int q) {
	if ((tokens[p].type != '(') || (tokens[q].type != ')')) { return false; }

	int count = 0;
	while (p <= q) {
		if (tokens[p].type == '(') {
			count++;
		} else if (tokens[p].type == ')') {
			count--;
		}

		if ((count == 0) && (p < q)) { return false; }
		
		p++;
	}

	if (count != 0) {return false; }
	return true;
}

int search_main_token_position (int p, int q) {
	int main_token_position = 0;
	int priority = 0;
	if (tokens[p].type == TK_DEREF) return p;

	while (q >= p){
		if (tokens[q].type == ')') {
			int right = q;
			q--;
			while (check_parentheses(q, right) == false) { q--; }
			continue;
		} else if ((tokens[q].type != '+') 
			&& (tokens[q].type != '-')
			&& (tokens[q].type != '*')
			&& (tokens[q].type != '/')
			&& (tokens[q].type != TK_EQ)
			&& (tokens[q].type != TK_NEQ)
			&& (tokens[q].type != TK_AND)) { 
			q--;
			continue; 
		}
		
		//Priority return to the rightmost plus and minus positions
		if (tokens[q].type == TK_AND) {
			main_token_position = q;
			priority = 0;
			q--;
		}
		else if ((tokens[q].type == TK_EQ) || (tokens[q].type == TK_NEQ)) {
			if ((main_token_position == 0) || (priority > 1)) {
				main_token_position = q;
				priority = 1;
			}
			q--;
		}
		else if ((tokens[q].type == '+') || (tokens[q].type == '-')) {
			if ((main_token_position == 0) || (priority > 2)) {
				main_token_position = q;
				priority = 2;
			}
			q--;
		} 
		else if ((tokens[q].type == '*') || (tokens[q].type == '/')) {
			if ((main_token_position == 0) || (priority > 3)) {
				main_token_position = q;
				priority = 3;
			}
			q--;
		}
	}
	return main_token_position;
}

#define BAD_EXPREESION -1
word_t isa_reg_str2val(const char *s, bool *success);
word_t paddr_read(paddr_t addr, int len);


word_t eval(int p, int q) {
  if (p > q) {
    /* Bad expression */
	return BAD_EXPREESION;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
	if (tokens[p].type == TK_REG) {
		//word_t isa_reg_str2val(const char *s, bool *success)
		char *reg_name = tokens[p].str + 1; 
		bool reg_f = true;
		word_t reg_val = isa_reg_str2val(reg_name, &reg_f);
		Assert(reg_f, "Register name '%s' is not available.", reg_name);
		return reg_val;
	}

	return strtol(tokens[p].str, NULL, 0);
  }
  else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1);
  }
//   else if ((p == q - 1) && (tokens[p].type == TK_DEREF)) {
// 	paddr_t addr = strtol(tokens[q].str, NULL, 16);
// 	return paddr_read(addr, 1);
//   }
  else {
    int op = search_main_token_position(p, q);
	int addr = 0;
	int val1 = 0;
	int val2 = 0;
	if(tokens[op].type == TK_DEREF) {
		addr = eval(op + 1, q);
	} else {
		val1 = eval(p, op - 1);
    	val2 = eval(op + 1, q);
	}

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
	  case TK_EQ: return val1 == val2;
	  case TK_NEQ: return val1 != val2;
	  case TK_AND: return val1 && val2;
	  case TK_DEREF: return paddr_read(addr, 1);
	       
      default: assert(0);
    }
  }
}
