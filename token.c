#include <stdio.h>
#include <stdlib.h>
#include "token.h"

int insert_node(tokenlist_t **l, double dval, char *sval, int ival, char *text, int cat, int rows, int column, char *filename){
	

	tokenlist_t *new_head = NULL;

	new_head = malloc(sizeof(tokenlist_t));
	new_head->t.text = text;
	new_head->t.dval = dval;
	new_head->t.sval = sval;
	new_head->t.ival = ival;
	new_head->t.category = cat;
	new_head->t.lineno = rows;

	new_head->t.filename = filename;

	if(new_head == NULL)
	{
		printf("malloc failed in insert_node()");
		return -2;
		//-2 is returned because in other places -1 is
		//returned but DOESNT mean an error
		//so I don't know what to do with my life anymore.

	}
	
	new_head->next = *l;
	*l = new_head;
	return 0;
}

void print_list(tokenlist_t *l){

	
	tokenlist_t *curr = l;
	while(1){//TODO: make dynamic, probably while(1)
		printf("text: %d %s\n", curr->t.category, curr->t.text);
		printf("value: %d %s %f\n", curr->t.ival, curr->t.sval, curr->t.dval);
		curr = curr->next;

		if(curr->next == NULL){
			
			break;
		}
	}
}

void dealloc_list(tokenlist_t *l){

  tokenlist_t* tmp;

   while (l != NULL)
    {
       tmp = l;
       l = l->next;
       free(tmp);
    }
}
