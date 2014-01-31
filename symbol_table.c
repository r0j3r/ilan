struct symbol_entry
{
    enum symbol_type t;
    union
    {
        struct keyword
        {
            enum token_id id; 
        }
        struct name
        {
            struct fat_pointer * lexeme;
            struct symbol_entry * next;
        }
        struct symbol_descriptor
        {
            enum symbol_kind kind;
            union 
            {
                struct func
                {
                    void * name;
                    struct symbol_entry * param_types;
                    void * adress;
                }                
                struct library
                {
                    struct fat_pointer * name;
                    void * address;
                }
                struct module
                {
                    struct fat_pointer * name;
                    void * address; 
                } 
                struct variable
                {
                    struct fat_pointer * name;
                    enum primitive_types t;
                    void * address;
                } 
                struct type_decl
                {
                    struct fat_pointer * name; 
                    struct symbol_entry * type_list;
                }
            }
            struct symbol_entry * parent; //scope
            struct symbol_entry * sibling; 
            struct symbol_entry * children; //do we need this? 
        } 
    }
    struct symbol_entry * next; //symbol table chain 
}


int
initialise_symbol_table()
{
    int i;
    not_found_desc = &default_desc;
    memset(not_found_desc, 0, sizeof(*not_found_desc));
    not_found_desc->n = not_found_desc;
    not_found_desc->t_type = t_not_found;
    for(i = 0; i < symbol_table_size; i++)
    {
        symbol_table[i] = not_found_desc;
    }
    //load keywords
    for(i = 0; reserved_words[i]; i++)
    {
        struct symbol_descriptor * desc;
        desc = reserved_words[i];
        put_in_symbol_table(&desc->lexeme, desc->lexeme_len, desc);
    }
    return 0;
}

int
put_in_symbol_table(struct fat_pointer * l, size_t len,
    struct symbol_descriptor * d)
{
     uint32_t i;  
         
     printf("put in symbol table\n");
     struct symbol_descriptor * n;
     i = hashword(l->buff, len, symbol_table_initval) & 0x3ff;
     n = symbol_table[i]; //get the dummy node
     if (n == n->n) //empty slot
     {
         struct symbol_descriptor * h;
         h = make_symbol_descriptor();
         h->t_type = t_not_found;
         h->n = d;
         d->n = h;
         symbol_table[i] = h;
     }
     else
     {
         d->n = n->n; //insert in head of circular list
         n->n = d;
     }
     return 0;
}

struct symbol_descriptor *
get_from_symbol_table(struct fat_pointer * l, size_t len)
{
     uint32_t i;
     struct symbol_descriptor * n;
             
     printf("get from symbol table\n");
     i = hashword(l->buff, len, symbol_table_initval) & 0x3ff;
     printf("computed hash index at %d\n", i);
     n = symbol_table[i]; //get head node
     if (n != n->n)
     {                                                                          
         printf("found chain at %d\n", i);
         //n->lexeme = *l;
         if (n->lexeme.buflen == 0)
         {
             n->lexeme.buff = malloc(l->buflen);
             printf("malloc head node lexeme buff\n");
         }
         else if (n->lexeme.buflen < l->buflen)
         {
             void * t;
             t = realloc(n->lexeme.buff, l->buflen);                            
             if (t)
             {
                 n->lexeme.buff = t;
             }
             else
                 exit(700);
             printf("realloc head node lexeme buff\n");
         }
         n->lexeme.buflen = l->buflen;
         memcpy(n->lexeme.buff,l->buff, len * sizeof(unsigned int));
         n->lexeme_len = len;                                                   
         n = n->n;
         printf("searching chains %p\n", n);
         while(memcmp(l->buff, n->lexeme.buff,
             len * sizeof(unsigned int)) != 0) //search chains
         {
             n = n->n;
             printf("searching chains %p\n", n);
         }
         if (n != symbol_table[i])
         {
             printf("found symbol desc at %p\n", n);                            
         }
    }
    else
        printf("found empty slot at %d\n", i);
    return n;
}
