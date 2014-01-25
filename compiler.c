#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int size_t;
typedef unsigned int uint32_t;
const int end_of_source = 0;
const int symbol_table_initval = 0x13371337;
unsigned int line_count = 0;
unsigned int line_pos = 0;
unsigned int symbol_count = 0;

const char r0[] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
              0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const char r00[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
              0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
const char r10[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
              0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
const char r20[] = {0xff,0xff,0xff,0xff,0x00,0x00,0x00,0xff,
              0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
const char r30[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
              0x00,0x00,0xff,0xff,0xff,0xff,0xff,0x00};
const char r50[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
              0x00,0x00,0x00,0xff,0xff,0xff,0xff,0x00};
const char r70[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
              0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff};
             
const char * const  punctuation_index[16] = {r00, r10, r20, r30, r0, r50, r0, r70, 
                                r0, r0, r0, r0, r0, r0, r0, r0};   
struct symbol_descriptor * make_symbol_descriptor();
int
punctuation_char(int c)
{
    return (punctuation_index[c >> 4])[c & 0xf];
}

enum token_id
{
    t_not_found,
    t_root,
    t_identifier,
    t_char_literal,
    t_string_literal,
    t_real_number,
    t_integer_number,
    t_newline,
    t_comma,
    t_point,
    t_double_point,
    t_triple_point,
    t_end_of_source,
    t_equals,
    t_error,
    t_question_mark,
    t_left_brace,
    t_right_brace,
    t_assignment,
    t_colon,
    t_left_paren,
    t_right_paren,
    t_slash,
    t_back_slash,
    t_asterisk,
    t_assignment,
    t_plus,
    t_minus,
    t_left_bracket,
    t_right_bracket,
    t_semicolon,
    t_tilde,
    t_exclamation_point,
    t_for,
    t_from,
    t_to,
    t_by,
    t_while,
    t_do,
    t_until,
    t_switch,
    t_case,
    t_fallthru,
    t_default,
    t_if,
    t_else,
    t_library,
    t_module,
    t_export,
    t_use,
    t_less_than,
    t_greater_than,
    t_shift_left,
    t_shift_right,
    t_typedef,
    t_struct,
    t_union,
    t_enum,
    t_private,
    t_public,
    t_and,
    t_or,
    t_not,
    t_mod,
    t_continue,
    t_break,
    t_return,
    t_in,
    t_var,
    t_func
};

enum symbol_kind
{
    s_procedure,
    s_array,
    s_set,
    s_record
};

struct fat_pointer
{
    void * buff;
    size_t buflen;
};

struct token
{
    enum token_id id;
    struct fat_pointer lexeme;
    size_t lexeme_len;
    struct symbol_entry * e;
};

struct symbol_descriptor
{
    
    struct symbol_descriptor * parent;
    struct symbol_descriptor * next;
};

 symbol_entry
{
    enum token_id id;
    struct fat_pointer lexeme;
    size_t lexeme_len;
    struct symbol_descriptor * d;
    struct symbol_entry * n; //chain link
};

struct ast_node;

struct adj_link
{
    struct adj_link * n;
    struct ast_node * a;
};

struct ast_node
{
    struct token * t;
    struct adj_link * a;
    struct ast_node * n;
};

size_t scanner_pos = 0;
struct fat_pointer source_text;
const int symbol_table_size = 1 << 10;
struct symbol_descriptor * symbol_table[1 << 10];
struct symbol_descriptor default_desc, *not_found_desc;
int put_in_symbol_table(struct fat_pointer * l, size_t len,  
    struct symbol_descriptor * d);

int
insert_in_array(struct fat_pointer * a, unsigned int index, unsigned int l)
{
    printf("insert in array with len %u at byte offset %u\n",  a->buflen, 
        index * 4 );
    if ((index * 4) >= a->buflen)
    {
        void * t;
        printf("growing array to %u\n", a->buflen * 2);
        t = realloc(a->buff, a->buflen * 2);
        printf("checking realloced buffer\n");
        if (t)
        {
            a->buff = t;
            a->buflen = a->buflen * 2;
        }
        else
        {
            printf("memory full\n");
            exit(1);
        }
    }

    ((unsigned int *)(a->buff))[index] = l;
    return index;
}


unsigned int
next_source_char()
{
    if (scanner_pos < source_text.buflen)
    {
        line_pos++;  
        return ((unsigned char *)(source_text.buff))[scanner_pos++];
    }    
    else
        return 0x00; //force invalid byte   
}

int k_for[] = {'f','o','r', 0};
struct symbol_descriptor a_for = {.t_type = t_for, 
                                  .lexeme.buff = k_for,
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 3,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_from[] = {'f','r','o','m', 0};
struct symbol_descriptor a_from = {.t_type = t_from, 
                                  .lexeme.buff = k_from,
                                  .lexeme.buflen = 5,
                                  .lexeme_len = 4,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_to[] = {'t','o', 0};
struct symbol_descriptor a_to = {.t_type = t_to, 
                                  .lexeme.buff = k_to,
                                  .lexeme.buflen = 3,
                                  .lexeme_len = 2,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_by[] = {'b','y', 0};
struct symbol_descriptor a_by = {.t_type = t_by, 
                                  .lexeme.buff = k_by,
                                  .lexeme.buflen = 3,
                                  .lexeme_len = 2,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_while[] = {'w','h','i','l','e', 0};
struct symbol_descriptor a_while = {.t_type = t_while, 
                                  .lexeme.buff = k_while,
                                  .lexeme.buflen = 6,
                                  .lexeme_len = 5,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_do[] = {'d','o',0};
struct symbol_descriptor a_do = {.t_type = t_do, 
                                  .lexeme.buff = k_do,
                                  .lexeme.buflen = 3,
                                  .lexeme_len = 2,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_until[] = {'u','n','t','i','l', 0}; 
struct symbol_descriptor a_until = {.t_type = t_until, 
                                  .lexeme.buff = k_until,
                                  .lexeme.buflen = 6,
                                  .lexeme_len = 5,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_if[] = {'i','f', 0};
struct symbol_descriptor a_if = {.t_type = t_if, 
                                  .lexeme.buff = k_if,
                                  .lexeme.buflen = 3,
                                  .lexeme_len = 2,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_else[] = {'e','l','s','e', 0};
struct symbol_descriptor a_else = {.t_type = t_else, 
                                  .lexeme.buff = k_else,
                                  .lexeme.buflen = 5,
                                  .lexeme_len = 4,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_switch[] = {'s','w','i','t','c','h', 0};
struct symbol_descriptor a_switch = {.t_type = t_switch, 
                                  .lexeme.buff = k_switch,
                                  .lexeme.buflen = 7,
                                  .lexeme_len = 6,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_case[] = {'c','a','s','e', 0};
struct symbol_descriptor a_case = {.t_type = t_case, 
                                  .lexeme.buff = k_case,
                                  .lexeme.buflen = 5,
                                  .lexeme_len = 4,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_fallthru[] = {'f','a','l','l','t','h','r','u', 0};
struct symbol_descriptor a_fallthru = {.t_type = t_fallthru, 
                                  .lexeme.buff = k_fallthru,
                                  .lexeme.buflen = 9,
                                  .lexeme_len = 8,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_default[] = {'d','e','f','a','u','l','t', 0};
struct symbol_descriptor a_default = {.t_type = t_default,  
                                  .lexeme.buff = k_default, 
                                  .lexeme.buflen = 8,
                                  .lexeme_len = 7,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_and[] = {'a','n','d', 0};
struct symbol_descriptor a_and = {.t_type = t_and,  
                                  .lexeme.buff = k_and, 
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 3,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_or[] = {'o','r', 0};
struct symbol_descriptor a_or = {.t_type = t_or,  
                                  .lexeme.buff = k_or, 
                                  .lexeme.buflen = 3,
                                  .lexeme_len = 2,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_not[] = {'n','o','t', 0};
struct symbol_descriptor a_not = {.t_type = t_not,  
                                  .lexeme.buff = k_not, 
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 3,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_mod[] = {'m','o','d', 0};
struct symbol_descriptor a_mod= {.t_type = t_mod,  
                                  .lexeme.buff = k_mod, 
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 3,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_func[] = {'f','u','n','c', };
struct symbol_descriptor a_func= {.t_type = t_func,  
                                  .lexeme.buff = k_func, 
                                  .lexeme.buflen = 5,
                                  .lexeme_len = 4,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_var[] = {'v','a','r', 0};
struct symbol_descriptor a_var= {.t_type = t_var,  
                                  .lexeme.buff = k_var, 
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 3,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_in[] = {'i','n', 0};
struct symbol_descriptor a_in= {.t_type = t_in,  
                                  .lexeme.buff = k_in, 
                                  .lexeme.buflen = 3,
                                  .lexeme_len = 2,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_module[] = {'m','o','d','u','l','e', 0};
struct symbol_descriptor a_module= {.t_type = t_module,  
                                  .lexeme.buff = k_module, 
                                  .lexeme.buflen = 7,
                                  .lexeme_len = 6,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_library[] = {'l','i','b','r','a','r','y', 0};
struct symbol_descriptor a_library= {.t_type = t_library,  
                                  .lexeme.buff = k_library, 
                                  .lexeme.buflen = 8,
                                  .lexeme_len = 7,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_typedef[] = {'t','y','p','e','d','e','f', 0};
struct symbol_descriptor a_typedef= {.t_type = t_typedef,  
                                  .lexeme.buff = k_typedef, 
                                  .lexeme.buflen = 8,
                                  .lexeme_len = 7,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_return[] = {'r','e','t','u','r','n', 0};
struct symbol_descriptor a_return= {.t_type = t_return,  
                                  .lexeme.buff = k_return, 
                                  .lexeme.buflen = 7,
                                  .lexeme_len = 6,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_continue[] = {'c','o','n','t','i','n','u','e', 0};
struct symbol_descriptor a_continue= {.t_type = t_continue,  
                                  .lexeme.buff = k_continue, 
                                  .lexeme.buflen = 9,
                                  .lexeme_len = 8,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_break[] = {'b','r','e','a','k', 0};
struct symbol_descriptor a_break= {.t_type = t_break,  
                                  .lexeme.buff = k_break, 
                                  .lexeme.buflen = 6,
                                  .lexeme_len = 5,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_public[] = {'p','u','b','l','i','c', 0};
struct symbol_descriptor a_public= {.t_type = t_public,  
                                  .lexeme.buff = k_public, 
                                  .lexeme.buflen = 7,
                                  .lexeme_len = 6,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_private[] = {'p','r','i','v','a','t','e', 0};
struct symbol_descriptor a_private= {.t_type = t_private,  
                                  .lexeme.buff = k_private, 
                                  .lexeme.buflen = 8,
                                  .lexeme_len = 7,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_use[] = {'u','s','e', 0};
struct symbol_descriptor a_use= {.t_type = t_use,  
                                  .lexeme.buff = k_use, 
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 3,
                                  .keyword = 0xff,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
int k_sentinel[] = {0}; 
struct symbol_descriptor a_sentinel= {.t_type = t_not_found,  
                                  .lexeme.buff = k_sentinel, 
                                  .lexeme.buflen = 4,
                                  .lexeme_len = 0,
                                  .keyword = 0x0,
                                  .s_type = 0x0,
                                  .s_kind = 0x0,
                                  .n = 0x0};
struct symbol_descriptor *reserved_words[] = {&a_for, &a_from, &a_to, 
    &a_by, &a_while, &a_do, &a_until,  &a_if, &a_else, &a_switch, 
    &a_case, &a_fallthru, &a_default,  &a_and, &a_or, &a_mod, &a_func, 
    &a_var, &a_in, &a_module, &a_library, &a_typedef, &a_return, 
    &a_continue, &a_break, &a_public, &a_private, &a_use, &a_sentinel};

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

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
    a -= c; a ^= rot(c, 4); c += b; \
    b -= a; b ^= rot(a, 6); a += c; \
    c -= b; c ^= rot(b, 8); b += a; \
    a -= c; a ^= rot(c,16); c += b; \
    b -= a; b ^= rot(a,19); a += c; \
    c -= b; c ^= rot(b, 4); b += a; \
}

#define final(a,b,c) \
{ \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
}

//32 bit fast hash  by bob jenkins
uint32_t
hashword(const uint32_t *k, size_t length, uint32_t initval)
{
    uint32_t a,b,c;
    
    a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;
    
    while(length > 3)
    {
        a += k[0];
        b += k[1];
        c += k[2];
        mix(a,b,c);
        length -= 3;
        k += 3;
    } 
    switch(length)
    {
    case 3: c += k[2]; //fallthru
    case 2: b += k[1]; //fallthru 
    case 1: a += k[0]; //fallthru
        final(a,b,c); 
    case 0:
        break;     
    }
    return c;
}

struct symbol_descriptor *
make_symbol_descriptor()
{
    struct symbol_descriptor * d;

    d = malloc(sizeof(*d));
    memset(d, 0, sizeof(*d));
    return d;
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

unsigned int 
nextchar()
{
    unsigned int c;
    c = next_source_char();
    for(;c > 0x00;)
    {
        if ((0x00 < c) & (c < 0x80)) 
        {
            return c;
        }
        else if ((0xc2 <= c) & (c <= 0xdf))
        {
            int c1;
            printf("2 byte sequence\n");
            //2 byte sequence
            c1 = next_source_char();
            if ((0x80 <= c1) & (c1 <= 0xbf))
            {
                printf("continuation byte\n");
                c = c & 0x1f;
                c = c << 6;
                c1 = c1 & 0x3f;
                c = c | c1;
                return c;
            }
            else //skip too short 
            {
                c = c1; 
                continue;
            }
        }
        else if ((0xe0 <= c) & (c <= 0xef))
        {
            //3 byte sequence
            int c1;
            printf("3 byte sequence\n");
            c1 = next_source_char();
            if ((0x80 <= c1) & (c1 <= 0xbf))
            {
                printf("continuation byte\n");
                c = c & 0x0f;
                c = c << 6;
                c1 = c1 & 0x3f;
                c = c | c1; 
            }
            else
            {
                c = c1;
                continue;
            }
            c1 = next_source_char();
            if ((0x80 <= c1) & (c1 <= 0xbf))
            {
                printf("continuation byte\n");
                c = c << 6;
                c1 = c1 & 0x3f;
                c = c | c1;
                return c;
            }
            else
            {
                c = c1;
                continue;
            }
        }
        else if ((0xf0 <= c) & (c <= 0xf4))
        {
            //4 byte sequence 
            int c1;
            printf("4 byte sequence\n");
            c1 = next_source_char();
            if ((0x80 <= c1) & (c1 <= 0xbf))
            {
                printf("continuation byte\n");
                c = c & 0x07;
                c = c << 6;
                c1 = c1 & 0x3f;
                c = c | c1;
            }
            else
            {
                c = c1;
                continue;
            }
            c1 = next_source_char();
            if ((0x80 <= c1) & (c1 <= 0xbf))
            {
                printf("continuation byte\n");
                c = c << 6;
                c1 = c1 & 0x3f;
                c = c | c1;
            }
            else
            {
                c = c1;
                continue;
            }
            c1 = next_source_char();
            if ((0x80 <= c1) & (c1 <= 0xbf))
            {
                printf("continuation byte\n");
                c = c << 6;
                c1 = c1 & 0x3f;
                c = c | c1;
                if (c <= 0x10ffff)
                    return c;
                else  //get rid of invalid unicode
                {
                    c = next_source_char();
                    continue;   
                }
            }
            else
            {
                c = c1;
                continue;
            }
        }
        else //invalid start byte
        {
            printf("invalid byte\n");
            c = next_source_char();   
        }
    }
    return end_of_source;        
}

struct token *
make_token()
{
    struct token * t;
    t = malloc(sizeof(*t));
    memset(t, 0, sizeof(*t));
    t->lexeme.buff = malloc(16);
    if (!t->lexeme.buff)
    {
        printf("memory full\n");
        exit(701);
    }
    t->lexeme.buflen = 16;
    return t;
}

unsigned int l; //lookahead

struct token *
lex()
{
    struct token * tok;
    printf("lex line %d lookahead %d\n", line_count, l);
    do
    {
        switch(l)
        {
            case ' ':
            case '\t':
                l = nextchar();
                continue;
            case '=':
                tok = make_token();
                tok->type = t_equals;
                l = nextchar();
                return tok;    
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':   
            case '6': 
            case '7':
            case '8':
            case '9':
                tok = make_token();
                tok->lexeme_len = 0;
                while(('0' <= l) & (l <= '9'))
                {
                    insert_in_array(&(tok->lexeme), tok->lexeme_len++, l);
                    l = nextchar();
                }
                if (l == '.')
                {   
                    insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                    l = nextchar();
                    if (('0' <= l) & (l <= '9'))
                    {
                        while(('0' <= l) & (l <= '9'))
                        {
                            insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                            l = nextchar();
                        }    
                        if (l == 'E')
                        {
                            insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                            l = nextchar();
                            if (('0' <= l) & (l <= '9'))
                            {
                                while(('0' <= l) & (l <= '9'))
                                {
                                    insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                                    l = nextchar();
                                }
                                tok->type = t_real_number;
                            }
                            else
                            {
                                //error
                                tok->type = t_error;
                            }    
                        }
                        else
                        {
                            tok->type = t_real_number;
                        }
                    }
                    else
                    {
                        //error
                        tok->type = t_error;
                    }
                }
                else
                {
                    tok->type = t_integer_number;
                }
                return tok;
            case '.':
                tok = make_token();
                l = nextchar();
                if (l == '.')
                {
                   l = nextchar();
                   if (l == '.')
                   {
                       tok->type = t_triple_point;
                       l = nextchar();
                   }
                   else
                   {
                       tok->type = t_double_point;
                   }
                }
                else
                {
                   tok->type = t_point; 
                }   
                return tok; 
            case '~':
                tok = make_token();
                tok->type = t_tilde;
                l = nextchar();
                return tok;    
            case '!':
                tok = make_token();
                tok->type = t_exclamation_point;
                l = nextchar();
                return tok;    
            case '<':
                tok = make_token();
                l = nextchar();
                if (l == '<')
                {
                    tok->type = t_shift_left;
                    l = nextchar();
                }
                else                 
                    tok->type = t_less_than;
                return tok;     
            case '>':
                tok = make_token();
                if (l == '>')
                {
                    tok->type = t_shift_right;
                    l = nextchar();
                }
                else                 
                    tok->type = t_greater_than;
                return tok;     
            case '?':
                tok = make_token();
                tok->type = t_question_mark;
                l = nextchar();
                return tok;     
            case '"':
                tok = make_token();
                l = nextchar();
                while(l != '"')
                {
                    insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                    l = nextchar();
                    if (l == '\\')
                    {
                        l = nextchar();
                        insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                        l = nextchar();
                    }
                }
                tok->type = t_string_literal;
                l = nextchar();
                return tok;
            case '\'':
                tok = make_token();
                l = nextchar();
                if (l == '\\')
                {
                    l = nextchar();
                }    
                insert_in_array(&tok->lexeme, tok->lexeme_len++, l); 
                l = nextchar();
                if (l == '\'')  
                {
                    l = nextchar();
                } 
                else
                {
                    //error
                    printf("error: ' expected\n");
                }
                tok->type = t_char_literal;
                return tok;
            case '{':
                tok = make_token();
                tok->type = t_left_brace;
                l = nextchar();
                return tok;
            case '}':
                tok = make_token();
                tok->type = t_right_brace;
                l = nextchar();
                return tok;        
            case ':':
                tok = make_token();
                l = nextchar();
                if (l == '=')
                {
                    l = nextchar();
                    tok->type = t_assignment;
                }  
                else
                {
                    tok->type = t_colon;
                } 
                return tok;
            case '(':
                tok = make_token();
                l = nextchar();
                tok->type = t_left_paren; 
                return tok;   
            case ')':
                tok = make_token();
                tok->type = t_right_paren;
                l = nextchar();
                return tok;
            case '+':
                tok = make_token();
                tok->type = t_plus;
                l = nextchar();
                return tok;
            case '-':
                tok = make_token();
                tok->type = t_minus;
                l = nextchar();
                return tok;
            case '*':
                tok = make_token();
                tok->type = t_asterisk;
                l = nextchar();
                return tok;   
            case '/':
                tok = make_token();
                tok->type = t_slash;
                l = nextchar();
                return tok;              
            case ',':
                tok = make_token();
                tok->type = t_comma;
                l = nextchar();
                return tok;   
            case '[':
                tok = make_token();
                tok->type = t_left_bracket;
                l = nextchar();
                return tok;
            case ']':
                tok = make_token(); 
                tok->type = t_right_bracket;
                l = nextchar();
                return tok;
            case ';':
                tok = make_token();
                tok->type = t_semicolon;
                l = nextchar();
                return tok;
            case '#':
                while((l != '\r') & (l != '\n')) //consume comments
                {
                    l = nextchar();
                }    
                //fallthru
            case '\r':
            case '\n':
                line_count++;
                line_pos = 0;
                tok = make_token();
                tok->type = t_newline;
                l = nextchar();
                return tok;
        }
        
        if ((0x20 < l) & (l <= 0x10ffff)) //everything
        {
             struct symbol_descriptor * desc;
             printf("getting identifier line pos %d\n", line_pos);
             tok = make_token();
             printf("lookahead %c\n", l);
             while(!punctuation_char(l))
             {
                 insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                 l = nextchar();   
                 printf("lookahead %c\n", l);
             }
             desc = get_from_symbol_table(&tok->lexeme, tok->lexeme_len);
             if (desc->keyword)
             {
                 printf("keyword found\n");
                 tok->type = desc->t_type;    
             }
             else
             {
                 printf("symbol found\n");
                 tok->type = t_identifier;
             }
             return tok;
        }   
        else if (l != 0) //eat unrecognized chars 
        {
            l = nextchar();
        }     
    }
    while(l != 0);  
      
    tok = make_token();
    tok->type = t_end_of_source;
    return tok;
}

struct ast_node *
make_ast_node()
{
    struct ast_node * a;
    
    a = malloc(sizeof(*a));
    memset(a, 0, sizeof(*a));
    return a;
}

struct ast_node *
parse_library(struct token * t)
{
    struct token * tok;
    struct ast_node * node;
    
    tok = lex();
    if (tok->type == t_identifier)
    {
        struct token * library_name_tok;
        library_name_tok = tok;        
        tok = lex();
        while (tok->type == t_newline)
            tok = lex();
        if (tok->type == t_left_brace)
        {
            while (tok->type == t_newline)
                tok = lex();
            do
            {
                tok = lex();
                switch(tok->type)
                {
                case t_use:
                    parse_library_use(tok);  
                    break;
                case t_export:
                    parse_library_export(tok);
                    break;
                case t_right_brace:
                    return node;
                    break;    
                default: 
                    printf("syntax error\n");
                    exit(100);
                } // switch(tok->type) 
            } while(tok->type != t_end_of_source);
        } // if (tok->type == t_left_brace)
    }    
    else
    {
        printf("syntax error\n");
        exit(100);
    }
}

struct ast_node *
parse_module(struct token * t)
{

}

struct ast_node *
parse() //we construct the AST
{
    struct ast_node * root;
    struct ast_node * node;
    struct token * tok;
    unsigned int i, count = 0;
   
    initialise_symbol_table(); 
    l = nextchar();
    root = make_ast_node();
    do
    {
        tok = lex();
        switch(tok->type)
        {
        case t_library: 
            node = parse_library(tok);    
            insert_node(node);
            add_link(root, node);
            break;
        case t_module:    
            node = parse_module(tok); 
            insert_node(node);
            add_link(root,node);
            break; 
        default:
            printf("syntax error\n");
            exit(100);      
        } //switch
    } //do 
    while(tok->type != t_end_of_source);
    
    for(i = 0; i < symbol_table_size; i++)
    {
        if (symbol_table[i] != not_found_desc)
            count++;
    }
    printf("%u symbols %u/%u slots used in symbol table\n", symbol_count, 
        count, symbol_table_size);
    return root;   
}

int
main(int argc, char * argv[])
{
   int fd;
   
   printf("arg : %d\n", argc);
   if (argc == 2)
   {
       struct stat statbuf;
       char * s;
        
       fd = open(argv[1], O_RDONLY);
       fstat(fd, &statbuf);
       s = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
       printf("statbuf.st_size: %u, buff %p\n", (unsigned int)statbuf.st_size, s);
       source_text.buff = s;
       source_text.buflen = statbuf.st_size;
   }
   else
   {
       return 1;  
   }      
   parse();
   return 0;
}
