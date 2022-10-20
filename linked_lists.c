#include "utils.h"

int add_cmd(cmd** head)
{
    cmd* n;
    cmd* p;

    n= (cmd*)calloc(1,sizeof(cmd));


    if(n==NULL)
        return 0;

    if(*head == NULL)                                /* if it's the first allocation, sets the head of the list to point at this node */
    {
        n->IC = 100;                                 /* sets the address */
        n->first_time = TRUE;
        *head = n;
        return 1;
    }
    p = *head;

    while (p->next!=NULL)
        p = (cmd*)p->next;                            /* if it isn't the first allocation, reaches to the last node and sets its 'next' to point at the new node */

    p->next = n;
    n->first_time  = FALSE;
    n->next = NULL;			              /* sets the last node to point at NULL */

    return 1;
}

int add_data(data_image** head)
{
    data_image* n;
    data_image* p;
    n= (data_image*)calloc(1,sizeof(data_image));

    if(n==NULL)
        return 0;

    n->DC = 0;                                        /* sets the address */
    if(*head==NULL)                                   /* if it's the first allocation, sets the head of the list to point at this node */
    {
        *head = n;
        n->first_time = TRUE;
        return 1;
    }
    p = *head;

    while (p->next!=NULL)
        p = (data_image*)p->next;                      /* if it isn't the first allocation, reaches to the last node and sets its 'next' to point at the new node */

    p->next = n;
    n->first_time = FALSE;
    n->next = NULL;			               /* sets the last node to point at NULL */

    return 1;
}

int add_symbol(symbol** head)
{
    symbol* n;
    symbol* p;
    n= (symbol*)calloc(1,sizeof(symbol));

    if(n==NULL)
        return 0;

    if(*head==NULL)                                /* if it's the first allocation, sets the head of the list to point at this node */
    {
        n->first_time = TRUE;
        *head = n;
        return 1;
    }
    p = *head;

    while (p->next!=NULL)
        p = (symbol*)p->next;                      /* if it isn't the first allocation, reaches to the last node and sets its 'next' to point at the new node */

    p->next = n;
    n->first_time = FALSE;
    n->next = NULL;			           /* sets the last node to point at NULL */

    return 1;
}
