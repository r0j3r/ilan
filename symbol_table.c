#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fat_pointer.h"
#include "token_id.h"
#include "symbol_entry.h"

const int symbol_table_initval = 0x13371337;

const int symbol_table_size = 1 << 10;
struct symbol_entry symbol_table[1 << 10];
struct symbol_entry default_desc, *not_found_desc;

extern struct symbol_entry *reserved_words[];

int
initialise_symbol_table()
{
    int i;

    memset(symbol_table, 0, sizeof(symbol_table));
    for(i = 0; i < symbol_table_size; i++)
    {
        symbol_table[i].next = &symbol_table[i]; 
    }
    //load keywords
    for(i = 0; reserved_words[i]; i++)
    {
        struct symbol_entry * entry;
        entry = reserved_words[i];
        put_in_symbol_table(entry);
    }
    return 0;
}

int
put_in_symbol_table(struct symbol_entry * e)
{
     unsigned int i;  
         
     printf("put in symbol table\n");
     struct symbol_entry * n;
     i = hashword(e->lexeme.buff, e->lexeme.buflen, symbol_table_initval) & 0x3ff;
     n = &symbol_table[i]; //get the dummy node

     e->next = n->next; //insert in head of circular list
     n->next = e;

     return 0;
}

struct symbol_entry *
get_from_symbol_table(struct fat_pointer * l, size_t len)
{
    unsigned int i;
    struct symbol_entry * n;
             
    printf("get from symbol table\n");
    i = hashword(l->buff, len, symbol_table_initval) & 0x3ff;
    printf("computed hash index at %d\n", i);
    n = &symbol_table[i]; //get head node

    //special case: empty list
    if (n == n->next) return 0;
                                                                          
    n->lexeme.buflen = l->buflen;
    n->lexeme.buff = l->buff;
    n->lexeme_len = len;                                                   
    n = n->next;

    printf("searching chains %p\n", n);
    while(memcmp(l->buff, n->lexeme.buff,
          len * sizeof(unsigned int)) != 0) //search chains
    {
        n = n->next;
        printf("searching chains %p\n", n);
    }
    if (n == &symbol_table[i])
        return 0;
    else
        return n;
}
