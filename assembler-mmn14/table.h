#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/*********************************** enums and structs ***********************************/

/* checks if a symbol refers to a line of code (instruction) or data */
typedef enum TypeSymbol {
    TYPE_UNK = 0,   /* .entry or .extern */
    TYPE_CODE = 1,  /* .data */
    TYPE_DATA = 2   /* .string */
} TypeSymbol;

char* str_symbol(TypeSymbol type); /* convert to str */

/*  checks if a symbol is declared in the current file or an external file */
typedef enum LocSym {
    LOC_UNK = 0,        /* .entry/.extern */
    LOC_ENTRY = 1,      /* .data */
    LOC_EXTERNAL = 2    /* .string */
} LocSym;

char* str_sym(LocSym loc); /* convert to str */

/* dynamic linked list for symbols */
typedef struct Symbol {
    char* label;
    int address;
    TypeSymbol type;
    LocSym loc;
    struct Symbol* next;
} Symbol;

/*********************************** Function Prototypes ***********************************/

/* Adds new error to the tail of the list -  Returns 1 if success, 0 if already exists) */
int sym_add(char* label, TypeSymbol type, LocSym loc);

/* Seatching the symbol in the table -  Returns NULL if not found  */
Symbol* symbol_search(char* label);

/* moves the addresses of data symbols by the number of words in the code section (IC) */
void data_move();

/* Updates  'entry' s in the talble which were declared by  an '.entry' directive in the source code */
void entry_sym_up(char* label);

/*  Write entry symbols to file:  */
void write_entry(char* file_path);

/* Free memory of symbol table */
void sym_t_free();

#endif
