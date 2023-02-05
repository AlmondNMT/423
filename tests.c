#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "utils.c"
#include "token.h"
#include "lists.c"




int main()
{
		tokenlist_t *list_head = NULL;
		list_head = malloc(sizeof(tokenlist_t));
		list_head->next = NULL;


        insert_node(&list_head, 8.5, "ghghg", 456, "yytext", 555, 343, "file");
        insert_node(&list_head, 7.5, "fef", 777, "htht", 990, 5655, "fole");
        insert_node(&list_head, 6.5, "yyy", 898, "ext", 7000, 6, "fowl");

        print_list(list_head);
}