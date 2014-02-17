#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "token_id.h"
#include "fat_pointer.h" 
#include "symbol_entry.h"

typedef unsigned int uint32_t;
const int end_of_source = 0;
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

int
punctuation_char(int c)
{
    return (punctuation_index[c >> 4])[c & 0xf];
}

struct token
{
    enum token_id id;
    struct fat_pointer lexeme;
    size_t lexeme_len;
    struct symbol_entry * e;
};

struct ast_node;

struct ast_node
{
    enum ast_id id;
    union
    {
        struct {} function_call;
        struct {} unary_op;
        struct {} binary_op;
        struct {} decl;
        struct {} block;
        struct {} expression;
        struct {} conditional;
    } op;
    struct ast_node * p; //parent
    struct ast_node * c; //children
    struct ast_node * s; //siblings
};

size_t scanner_pos = 0;
struct fat_pointer source_text;

int put_in_symbol_table(struct symbol_entry * );
struct symbol_entry *get_from_symbol_table(struct fat_pointer * , size_t);

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

struct symbol_entry *
make_symbol_entry(struct token * t, enum symbol_kind kind)
{
    struct symbol_entry * d;

    d = malloc(sizeof(*d));
    memset(d, 0, sizeof(*d));
    d->lexeme = t->lexeme;
    d->lexeme_len = t->lexeme_len;
    d->k = kind;
    return d;
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

unsigned int l; //character lookahead

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
                tok->id = t_equals;
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
                                tok->id = t_real_number;
                            }
                            else
                            {
                                //error
                                tok->id = t_error;
                            }    
                        }
                        else
                        {
                            tok->id = t_real_number;
                        }
                    }
                    else
                    {
                        //error
                        tok->id = t_error;
                    }
                }
                else
                {
                    tok->id = t_integer_number;
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
                       tok->id = t_triple_point;
                       l = nextchar();
                   }
                   else
                   {
                       tok->id = t_double_point;
                   }
                }
                else
                {
                   tok->id = t_point; 
                }   
                return tok; 
            case '~':
                tok = make_token();
                tok->id = t_tilde;
                l = nextchar();
                return tok;    
            case '!':
                tok = make_token();
                tok->id = t_exclamation_point;
                l = nextchar();
                return tok;    
            case '<':
                tok = make_token();
                l = nextchar();
                if (l == '<')
                {
                    tok->id = t_shift_left;
                    l = nextchar();
                }
                else                 
                    tok->id = t_less_than;
                return tok;     
            case '>':
                tok = make_token();
                if (l == '>')
                {
                    tok->id = t_shift_right;
                    l = nextchar();
                }
                else                 
                    tok->id = t_greater_than;
                return tok;     
            case '?':
                tok = make_token();
                tok->id = t_question_mark;
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
                tok->id = t_string_literal;
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
                tok->id = t_char_literal;
                return tok;
            case '{':
                tok = make_token();
                tok->id = t_left_brace;
                l = nextchar();
                return tok;
            case '}':
                tok = make_token();
                tok->id = t_right_brace;
                l = nextchar();
                return tok;        
            case ':':
                tok = make_token();
                l = nextchar();
                if (l == '=')
                {
                    l = nextchar();
                    tok->id = t_assignment;
                }  
                else
                {
                    tok->id = t_colon;
                } 
                return tok;
            case '(':
                tok = make_token();
                l = nextchar();
                tok->id = t_left_paren; 
                return tok;   
            case ')':
                tok = make_token();
                tok->id = t_right_paren;
                l = nextchar();
                return tok;
            case '+':
                tok = make_token();
                tok->id = t_plus;
                l = nextchar();
                return tok;
            case '-':
                tok = make_token();
                tok->id = t_minus;
                l = nextchar();
                return tok;
            case '*':
                tok = make_token();
                tok->id = t_asterisk;
                l = nextchar();
                return tok;   
            case '/':
                tok = make_token();
                tok->id = t_slash;
                l = nextchar();
                return tok;              
            case ',':
                tok = make_token();
                tok->id = t_comma;
                l = nextchar();
                return tok;   
            case '[':
                tok = make_token();
                tok->id = t_left_bracket;
                l = nextchar();
                return tok;
            case ']':
                tok = make_token(); 
                tok->id = t_right_bracket;
                l = nextchar();
                return tok;
            case ';':
                tok = make_token();
                tok->id = t_semicolon;
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
                tok->id = t_newline;
                l = nextchar();
                return tok;
        }
        
        if ((0x20 < l) & (l <= 0x10ffff)) //everything
        {
             struct symbol_entry * entry;
             printf("getting identifier line pos %d\n", line_pos);
             tok = make_token();
             printf("lookahead %c\n", l);
             while(!punctuation_char(l))
             {
                 insert_in_array(&tok->lexeme, tok->lexeme_len++, l);
                 l = nextchar();   
                 printf("lookahead %c\n", l);
             }
             entry = get_from_symbol_table(&tok->lexeme, tok->lexeme_len);
             if (entry == 0)
             {
                 struct symbol_entry * new_entry;
                 new_entry = make_symbol_entry(tok, s_identifier);
                 put_in_symbol_table(new_entry); 
                 tok->e = new_entry; 
                 tok->id = t_identifier; 
             }
             else
             {
                 if (entry->k == s_keyword)
                 {
                     printf("keyword found\n");
                     tok->id = entry->sym.keyword.id;    
                 }
                 else
                 {
                     printf("symbol found\n");
                     tok->id = t_identifier;
                     tok->e = entry; 
                 }
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
    tok->id = t_end_of_source;
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
    if (tok->id == t_identifier)
    {
        struct token * library_name_tok;
        library_name_tok = tok;        
        tok = lex();
        while (tok->id == t_newline)
            tok = lex();
        if (tok->id == t_left_brace)
        {
            while (tok->id == t_newline)
                tok = lex();
            do
            {
                tok = lex();
                switch(tok->id)
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
                } // switch(tok->id) 
            } while(tok->id != t_end_of_source);
        } // if (tok->id == t_left_brace)
    }    
    else
    {
        printf("syntax error\n");
        exit(100);
    }
}

struct ast_node *
parse_declaration(struct token * lookahead)
{
    struct token * tok;
    tok = lex();
    switch(tok->id)
    {
    case t_library:
        break;
    case t_module:
        break;
    case t_var:
        break;
    case t_const:
        break;
    case t_func:
        parse_function_decl();
        break;
    case t_typedef:
        break;
    case t_struct:
        break;
    case t_enum:
        break;
    case t_union:
        break;
    default:
        break; 
    }
}

struct ast_node *
parse_module(struct token * tok_lookahead)
{
    struct ast_node * n;
    struct token * tok;

    if (tok_lookahead->id == t_colon)
    {
        tok = lex(); //get the module name
        if (tok->id == t_identifier)
        {
            tok = lex(); 
            while(tok->id != end_of_source)
            {
                n = parse_declaration(tok);
            }
        }
        else
        {}
    }
}

struct ast_node *
parse() //we construct the AST
{
    struct ast_node * root;
    struct ast_node * node;
    struct token * tok;
    unsigned int i, count = 0;
   
    initialise_symbol_table(); 
    initialise_scanner(); //l = nextchar();
    root = make_ast_node();
    do
    {
        tok = lex();
        switch(tok->id)
        {
        case t_module: 
            tok = lex();   
            node = parse_module(tok); 
            insert_node(node);
            add_link(root,node);
            break; 
        default:
            printf("syntax error\n");
            exit(100);      
        } //switch
    } //do 
    while(tok->id != t_end_of_source);
    
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
