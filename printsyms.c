#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include "punygram.tab.h"

void printsymbol(char *s);

void printsyms(struct tree *t)
{
    if(t == NULL)
        return;
    if(t->nkids > 0) {
        for(int i = 0; i<t->nkids; i++)
            printsyms(t->kids[i]);
    } else {
        if(t->leaf->category == NAME)
            printsymbol(t->leaf->text);
    }
}

void printsymbol(char *s)
{
   printf("%s\n", s); fflush(stdout);
}
