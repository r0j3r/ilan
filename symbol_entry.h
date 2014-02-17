
enum symbol_kind
{
    s_keyword,
    s_identifier
};

enum symbol_desc_kind
{
    d_func,
    d_library,
    d_module,
    d_var
};

struct symbol_descriptor
{
    enum symbol_desc_kind kind;
    union
    {
        struct
        {
            void * name;
            struct symbol_entry * param_types;
            void * adress;
        } func;                
        struct
        {
            struct fat_pointer * name;
            void * address;
        } library;
        struct 
        {
            struct fat_pointer * name;
            void * address; 
        } module; 
        struct 
        {
            struct fat_pointer * name;
            struct symbol_entry * types;
            void * address;
        } variable;
        struct 
        {
            struct fat_pointer * name; 
            struct symbol_entry * type_list;
        } type_decl;
    } desc;
    struct symbol_entry * parent; //scope
    struct symbol_entry * sibling; 
    struct symbol_entry * children; //do we need this? 
};

struct symbol_entry
{
    enum symbol_kind k;
    struct fat_pointer lexeme;
    int lexeme_len;
    union
    {
        struct
        {
            enum token_id id;
        } keyword;
        struct 
        {
            struct symbol_descriptor * desc;
        } identifier;
    } sym;
    struct symbol_entry * next; //symbol table chain 
};
