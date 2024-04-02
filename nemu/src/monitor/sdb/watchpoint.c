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

#include "sdb.h"

#define NR_WP 32

// typedef struct watchpoint {
//   int NO;
//   struct watchpoint *next;

//   /* TODO: Add more members if necessary */
//   bool en;
// //   int addr;
//   word_t val;
//   int hit_count;
//   char *EXPR;
// } WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
	wp_pool[i].en = 0;
	// wp_pool[i].addr = 0;
	wp_pool[i].val = 0;
	wp_pool[i].hit_count = 0;
	wp_pool[i].EXPR = NULL;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	if (free_ == NULL) Assert(0, "No idle watchpoint structure\n");
	
	WP *new_ = free_;
	free_ = free_->next;
	new_->next = head;
	head = new_;
	new_->en = 1;

	printf("new success\n");
	return new_;
};

void free_wp(WP *wp) {
	WP *wp_front = head;
	while (wp_front->next != wp && wp_front != NULL) {
		wp_front = wp_front->next;
	}

	if (wp_front == NULL) assert(0);
	wp_front->next = wp->next;

	free(wp);
	printf("free success\n");
};

WP* find_wp (int wp_no, bool *success) {
	WP *find = head;
	if (find == NULL) {
		printf("There are no watchpoint in place.\n");
		success = false;
		return NULL;
	}
	else {
		while (wp_no != find->NO) find = find->next;
	}

	if (find == NULL) {
		success = false;
		return NULL;
	}

	return find;
}

void scanf_wp_head (bool *hit) {
	WP *scanf_wp = head;
	bool success;
	while (scanf_wp != NULL) {
		word_t cur_expr_res = expr(scanf_wp->EXPR, &success);
		assert(success);
		if (scanf_wp->val != cur_expr_res) {
			*hit = true;
			printf("\nwp no.%d\n", scanf_wp->NO);
			printf("Old value: %u \t %#x \n", scanf_wp->val, scanf_wp->val);
			printf("New value: %u \t %#x \n", cur_expr_res, cur_expr_res);
			scanf_wp->val = cur_expr_res;
			scanf_wp->hit_count++;
		}
	}
}

