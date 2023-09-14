#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coder.h"
#include "files.h"
#include "table.h"

/* Symbols will be stored as a dynamic linked list */
struct Symbol* symbol_table = NULL;
struct Symbol* tail = NULL; /* to insert at the end without traversal */

/* enum to_string converter */
char* str_symbol(TypeSymbol sym_type) {
    switch (sym_type) {
        case TYPE_UNK: return "N/A";
        case TYPE_CODE:   return "CODE";
        case TYPE_DATA: return "DATA";
        default: return "Unknown SymType";
    }
}

/* enum to_string converter */
char* str_sym(LocSym sym_loc) {
    switch (sym_loc) {
        case LOC_UNK: return "N/A";
        case LOC_ENTRY:   return "ENTRY";
        case LOC_EXTERNAL: return "EXTERNAL";
        default: return "Unknown SymLoc";
    }
}

/* Internal function used by lookup_symbol and add_symbol */
Symbol* set_sym(char* label) {
    Symbol* symbol;
    symbol = symbol_table;
    while (symbol != NULL) {
        if (strcmp(symbol->label, label) == 0) {
            return symbol;
        }
        symbol = symbol->next;
    }
    return NULL;
}

/* Adds a new symbol to the tail of the list */
int sym_add(char* label, TypeSymbol type, LocSym loc) {
    struct Symbol* new_symbol;

    /* First check this symbol doesn't already exist */
    if (set_sym(label) != NULL) {
        errors_num++;
        printf("Error in line %i: Symbol \'%s\' already exists\n", line_number, label);
        return 0;
    }

    new_symbol = (Symbol*)malloc(sizeof(Symbol));
    new_symbol->label = label;
    if (loc == LOC_EXTERNAL) {
        new_symbol->address = 0;
    }
    else if (type == TYPE_CODE) {
        new_symbol->address = get_IC();
    }
    else { /* data */
        new_symbol->address = get_DC();
    }

    new_symbol->type = type;
    new_symbol->loc = loc;
    new_symbol->next = NULL;

    if (symbol_table == NULL) {
        symbol_table = new_symbol;
        tail = new_symbol;
    }
    else {
        tail->next = new_symbol;
        tail = new_symbol;
    }
    return 1;
}

/* Searching symbol in the table - Returns NULL if not found. */
Symbol* symbol_search(char* label) {
    Symbol* symbol;
    symbol = set_sym(label);
    if (symbol == NULL) {
        printf("Error in line %i: Unrecognized symbol \'%s\'\n", line_number, label);
        errors_num++;
    }
    return symbol;
}

/* Moving the addresses of data symbols by IC (the number of words in the code section) */
void data_move() {
    Symbol* symbol;
    symbol = symbol_table;
    while (symbol != NULL) {
        if (symbol->type == (int)DATA) {
            symbol->address += get_IC();
        }
        symbol = symbol->next;
    }
}

/* Updates 'entry' in the symbols in the table */
void entry_sym_up(char* label) {
    Symbol *symbol;
    symbol = symbol_search(label);
    if (symbol != NULL) {
        symbol->loc = LOC_ENTRY;
    }
}

/* Write Symbol table to .ent file */
void write_entry(char* file_path) {
    FILE *fp;
    Symbol* symbol;
    symbol = symbol_table;
    fp = fopen(file_path, "w");
    if (fp) {
        while (symbol != NULL) {
            if (symbol->loc == LOC_ENTRY) {
                fprintf(fp, "%s ", symbol->label);
                address_to_file(fp, symbol->address);
                fprintf(fp, "\n");
            }
            symbol = symbol->next;
        }
        fclose(fp);
    }
    else {
        errors_num++;
    }
}

/* Free symbol table memory */
void sym_t_free() {
    Symbol* tmp;
    while (symbol_table != NULL) {
        tmp = symbol_table;
        symbol_table = symbol_table->next;
        free(tmp);
    }
}
