#include <stdlib.h>
#include "token_id.h"
#include "fat_pointer.h"
#include "symbol_entry.h"

int k_for[] = {'f','o','r', 0};
struct symbol_entry a_for = {.sym.keyword.id = t_for, 
                             .lexeme.buff = k_for,
                             .lexeme.buflen = 4,
                             .lexeme_len = 3,
                             .next = 0,
                             .k = s_keyword
                            };
int k_from[] = {'f','r','o','m', 0};
struct symbol_entry a_from = {.sym.keyword.id = t_from, 
                              .lexeme.buff = k_from,
                              .lexeme.buflen = 5,
                              .lexeme_len = 4,
                             .next = 0,
                             
                              .k = s_keyword
                              };
int k_to[] = {'t','o', 0};
struct symbol_entry a_to = {.sym.keyword.id = t_to, 
                            .lexeme.buff = k_to,
                            .lexeme.buflen = 3,
                            .lexeme_len = 2,
                             .next = 0,
                             
                                 .k = s_keyword
                                };
int k_by[] = {'b','y', 0};
struct symbol_entry a_by = {.sym.keyword.id = t_by, 
                            .lexeme.buff = k_by,
                            .lexeme.buflen = 3,
                            .lexeme_len = 2,
                             .next = 0,
                             
                                 .k = s_keyword
                                };
int k_while[] = {'w','h','i','l','e', 0};
struct symbol_entry a_while = {.sym.keyword.id = t_while, 
                              .lexeme.buff = k_while,
                              .lexeme.buflen = 6,
                              .lexeme_len = 5,
                              .next = 0,
                              .k = s_keyword
                                  };
int k_do[] = {'d','o',0};
struct symbol_entry a_do = {.sym.keyword.id = t_do, 
                               .lexeme.buff = k_do,
                               .lexeme.buflen = 3,
                               .lexeme_len = 2,
                             .next = 0,
        
                                 .k = s_keyword
                                };
int k_until[] = {'u','n','t','i','l', 0}; 
struct symbol_entry a_until = {.sym.keyword.id = t_until, 
                               .lexeme.buff = k_until,
                               .lexeme.buflen = 6,
                               .lexeme_len = 5,
                                  .k = s_keyword,
                               .next = 0,
                                  };
int k_if[] = {'i','f', 0};
struct symbol_entry a_if = {.sym.keyword.id = t_if, 
                               .lexeme.buff = k_if,
                               .lexeme.buflen = 3,
                               .lexeme_len = 2,
                                  .k = s_keyword,
                                 .next = 0
                                  };
int k_else[] = {'e','l','s','e', 0};
struct symbol_entry a_else = {.sym.keyword.id = t_else, 
                               .lexeme.buff = k_else,
                               .lexeme.buflen = 5,
                               .lexeme_len = 4,
                              .next = 0,
                                  .k = s_keyword};
int k_switch[] = {'s','w','i','t','c','h', 0};
struct symbol_entry a_switch = {.sym.keyword.id = t_switch, 
                               .lexeme.buff = k_switch,
                               .lexeme.buflen = 7,
                               .lexeme_len = 6,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_case[] = {'c','a','s','e', 0};
struct symbol_entry a_case = {.sym.keyword.id = t_case, 
                               .lexeme.buff = k_case,
                               .lexeme.buflen = 5,
                               .lexeme_len = 4,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_fallthru[] = {'f','a','l','l','t','h','r','u', 0};
struct symbol_entry a_fallthru = {.sym.keyword.id = t_fallthru, 
                               .lexeme.buff = k_fallthru,
                               .lexeme.buflen = 9,
                               .lexeme_len = 8,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_default[] = {'d','e','f','a','u','l','t', 0};
struct symbol_entry a_default = {.sym.keyword.id = t_default,  
                               .lexeme.buff = k_default, 
                               .lexeme.buflen = 8,
                               .lexeme_len = 7,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_and[] = {'a','n','d', 0};
struct symbol_entry a_and = {.sym.keyword.id = t_and,  
                               .lexeme.buff = k_and, 
                               .lexeme.buflen = 4,
                               .lexeme_len = 3,
                                 
                              .next = 0, 
                              .k = s_keyword
                                  };
int k_or[] = {'o','r', 0};
struct symbol_entry a_or = {.sym.keyword.id = t_or,  
                               .lexeme.buff = k_or, 
                               .lexeme.buflen = 3,
                               .lexeme_len = 2,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_not[] = {'n','o','t', 0};
struct symbol_entry a_not = {.sym.keyword.id = t_not,  
                               .lexeme.buff = k_not, 
                               .lexeme.buflen = 4,
                               .lexeme_len = 3,
                              .next = 0,
                                  .k = s_keyword
                                 };
int k_mod[] = {'m','o','d', 0};
struct symbol_entry a_mod= {.sym.keyword.id = t_mod,  
                               .lexeme.buff = k_mod, 
                               .lexeme.buflen = 4,
                               .lexeme_len = 3,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_func[] = {'f','u','n','c', };
struct symbol_entry a_func= {.sym.keyword.id = t_func,  
                               .lexeme.buff = k_func, 
                               .lexeme.buflen = 5,
                               .lexeme_len = 4,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_var[] = {'v','a','r', 0};
struct symbol_entry a_var= {.sym.keyword.id = t_var,  
                               .lexeme.buff = k_var, 
                               .lexeme.buflen = 4,
                               .lexeme_len = 3,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_in[] = {'i','n', 0};
struct symbol_entry a_in= {.sym.keyword.id = t_in,  
                               .lexeme.buff = k_in, 
                               .lexeme.buflen = 3,
                               .lexeme_len = 2,
                              .next = 0,
                                  .k = s_keyword
                               };
int k_module[] = {'m','o','d','u','l','e', 0};
struct symbol_entry a_module= {.sym.keyword.id = t_module,  
                               .lexeme.buff = k_module, 
                               .lexeme.buflen = 7,
                               .lexeme_len = 6,
                              .next = 0,
                                  .k = s_keyword
                                  };
int k_library[] = {'l','i','b','r','a','r','y', 0};
struct symbol_entry a_library= {.sym.keyword.id = t_library,  
                               .lexeme.buff = k_library, 
                               .lexeme.buflen = 8,
                               .lexeme_len = 7,
                              .next = 0,
                                  .k = s_keyword};
int k_typedef[] = {'t','y','p','e','d','e','f', 0};
struct symbol_entry a_typedef= {.sym.keyword.id = t_typedef,  
                               .lexeme.buff = k_typedef, 
                               .lexeme.buflen = 8,
                               .lexeme_len = 7,
                              .next = 0,
                                  .k = s_keyword};
int k_return[] = {'r','e','t','u','r','n', 0};
struct symbol_entry a_return= {.sym.keyword.id = t_return,  
                               .lexeme.buff = k_return, 
                               .lexeme.buflen = 7,
                               .lexeme_len = 6,
                              .next = 0,
                                  .k = s_keyword};
int k_continue[] = {'c','o','n','t','i','n','u','e', 0};
struct symbol_entry a_continue= {.sym.keyword.id = t_continue,  
                               .lexeme.buff = k_continue, 
                               .lexeme.buflen = 9,
                               .lexeme_len = 8,
                              .next = 0,
                                  .k = s_keyword};
int k_break[] = {'b','r','e','a','k', 0};
struct symbol_entry a_break= {.sym.keyword.id = t_break,  
                               .lexeme.buff = k_break, 
                               .lexeme.buflen = 6,
                               .lexeme_len = 5,
                              .next = 0,
                                  .k = s_keyword};
int k_public[] = {'p','u','b','l','i','c', 0};
struct symbol_entry a_public= {.sym.keyword.id = t_public,  
                               .lexeme.buff = k_public, 
                               .lexeme.buflen = 7,
                               .lexeme_len = 6,
                              .next = 0,
                                  .k = s_keyword};
int k_private[] = {'p','r','i','v','a','t','e', 0};
struct symbol_entry a_private= {.sym.keyword.id = t_private,  
                               .lexeme.buff = k_private, 
                               .lexeme.buflen = 8,
                               .lexeme_len = 7,
                              .next = 0,
                                  .k = s_keyword};
int k_use[] = {'u','s','e', 0};
struct symbol_entry a_use= {.sym.keyword.id = t_use,  
                               .lexeme.buff = k_use, 
                               .lexeme.buflen = 4,
                               .lexeme_len = 3,
                              .next = 0,
                                  .k = s_keyword};
int k_sentinel[] = {0}; 
struct symbol_entry a_sentinel= {.sym.keyword.id = t_not_found,  
                               .lexeme.buff = k_sentinel, 
                               .lexeme.buflen = 4,
                               .lexeme_len = 0,
                              .next = 0,
                                  .k = s_keyword
                                  };
struct symbol_entry *reserved_words[] = {&a_for, &a_from, &a_to, 
    &a_by, &a_while, &a_do, &a_until,  &a_if, &a_else, &a_switch, 
    &a_case, &a_fallthru, &a_default,  &a_and, &a_or, &a_mod, &a_func, 
    &a_var, &a_in, &a_module, &a_library, &a_typedef, &a_return, 
    &a_continue, &a_break, &a_public, &a_private, &a_use, &a_sentinel};
