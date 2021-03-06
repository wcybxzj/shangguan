#include <stdio.h>
#include <stdlib.h>

#include "mylist.h"

#define NAMESIZE 32

struct stu_st {
	int id;
	char name[NAMESIZE];
	struct list_head node;
};

int main(void)
{
	LIST_HEAD(head);	
	struct stu_st *data = NULL;
	struct stu_st *cur = NULL;
	struct list_head *pos = NULL;
	int i;
	char str[NAMESIZE] = "shagua5";

	for (i = 1; i <= 10; i++) {
		data = malloc(sizeof(*data));
		if (NULL == data)
			break;
		data->id = rand()%10+i;
		snprintf(data->name, NAMESIZE, "shagua%d", i);

		list_add_tail(&data->node, &head);
	}

	printf("list_for_each\n");
	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		printf("%-3d%s\n", cur->id, cur->name);
	}

	/*
 for (data = ({ const typeof( ((typeof(*data) *)0)->node ) *__mptr = (&head->next); (typeof(*data) *)( (char *)__mptr - ((size_t) &((typeof(*data) *)0)->node) );}); &data->node != (&head); data = list_next_entry(data, node)){
   printf("%-3d%s\n",data->id, data->name);
  }
  */

	printf("list_for_each_entry\n");
	list_for_each_entry(data, &head, node){
		printf("%-3d%s\n",data->id, data->name);
	}


	//delete
	printf("*********delete********\n");
	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		if (strcmp(cur->name, str) == 0) {
			list_del(&cur->node);
			break;
		}
	}
	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		printf("%-3d%s\n", cur->id, cur->name);
	}

	list_for_each(pos, &head) {
		cur = list_entry(pos, struct stu_st, node);
		free(cur);
	}

	return 0;
}
