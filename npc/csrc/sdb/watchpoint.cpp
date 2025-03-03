#include "include/common.h"
#include "sdb.h"

#define NR_WP 32

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
	memset(wp_pool[i].EXPR, '\0', sizeof(wp_pool[i].EXPR));
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
	while (wp_front->next != wp && wp_front->next != NULL) {
		wp_front = wp_front->next;
	}

	if (wp_front == NULL) assert(0);

	if (wp_front == wp) {
		head = NULL;
	}
	else{
		wp_front->next = wp->next;
		wp->next = NULL;
	}

	printf("free success\n");
};

WP* find_wp (int wp_no, bool *success) {
	WP *find = head;
	if (find == NULL) {
		printf("There are no watchpoint in place.\n");
		*success = false;
		return NULL;
	}
	else {
		while (wp_no != find->NO) find = find->next;
	}

	if (find == NULL) {
		*success = false;
		return NULL;
	}

	return find;
}

void scanf_wp_head (bool *hit) {
	WP *scanf_wp = head;
	bool success = true;
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
		scanf_wp = scanf_wp->next;
	}
}

void wp_display () {
	printf("NO \ten \tval \thit count \tEXPR\n");
	WP *p = head;
	if (p == NULL) {
		printf("There are no watchpoint in place.\n");
		return;
	}
	else {
		while (p != NULL) {
			printf("%d \t%d \t%d \t%d \t\t%s\n", p->NO, p->en, p->val, p->hit_count, p->EXPR);
			p = p->next;
		}
	}
}
