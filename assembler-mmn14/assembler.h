#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>

#include "table.h"

/*********************************** Constants ***********************************/
/* num of ops */
#define NUMBER_OF_OPS 16

/*Start address of the instruction image  */
#define START_ADDRESS 100

/* Memory allocate for a line */
#define LINE_LEN 2056

/*Input line Max length  */
#define MAX_LEN 80

/* num of registers */
#define NUMBER_OF_REG 8

/* num of directives */
#define NUMBER_OF_DIREC 4

/* Number of input lines to allocate for Each time the amount allocated is exceeded, another 'batch' is dynamically reallocated */
#define INPUT_SIZE 1024



/*********************************** Data structures and function prototypes ***********************************/


/* Argument types that used by the various directives */
typedef enum ArgType {
    LABEL = 0,  /* .entry or .extern */
    INT = 1,    /* .data */
    STRING = 2  /* .string */
} ArgType;

char* to_str(ArgType arg_type); /* enum to str */

/* IMMEDIATE(0),DIRECT(1),RELATIVE(2), REGISTER(3)
(0) integer value prefixed with '#' 
(1) a label, to be translated into the address in the code image where it is declared
(2) a label, prefixed by '&', denoting the (pos/neg) distance (in 'words') of the current instruction from the address where the specified label is declared 
(3) the name of a register ('r0'-'r7')  */
typedef enum addressing_type {
    IMMEDIATE = 0,
    DIRECT = 1,
    RELATIVE = 2,
    REGISTER = 3
} addressing_type;

char* addr_mode_str(addressing_type arg_type); /* enum to str */

/* Each input line is parsed, checked for syntax, and restructured into the following structure before entering the 2-pass assembler stage */

typedef struct ParsedLine {
    int number_line;
    char* label; /* optional */
    char* op; /* relevant iff the input line is one of the 16 assembler 'operations' */
    char* directive;  /* relevant iff the input line is one of the 4 assembler 'directives' */
    int num_of_args; /* the number of (comma-separated) args specified */
    char** args;  /* the (comma-separated) argument(s) which followed the op/directive on the input line */
} ParsedLine;

/*  This will store information concerning label references during the first pass that will then be used in the second pass to fill the missing pieces.
 * Also used to write the .ext file */
typedef struct InfoSym {
    int number_line;
    unsigned int IC;
    char* label;
    addressing_type adder_type;
} InfoSym;

/* InfoLink : A (absolute) address will not change - R (relocatable) internal address  will change in the loading stage - E (external) address will be determined in the loading stage */
typedef enum InfoLink {
    Linker_UNK = 0, /* (000) */
    E = 1,   /* (001) */
    R = 2,   /* (010) */
    A = 4   /* (100) */
} InfoLink;

/*  Contains the information for a code instruction word to be encoded into hex and generated as output  */
typedef struct Instruction {
    int opcode;
    addressing_type first_arg_type;
    int first_reg;
    addressing_type second_arg_type;
    int second_reg;
    int funct;
    InfoLink info_linker;
} Instruction;

/* Contains the information for a code operand word to be encoded into hex and generated as output */
typedef struct Operand {
    int value;  /* either an address or a literal numeric value */
    InfoLink info_linker;
} Operand;

/*  Keeps track of the types of each word entered into the code output array  */
typedef enum WordType {
    INSTRUCTION = 0,
    OPERAND = 1,
    DATA = 2
} WordType;

/* Represents a line of code that will be generated - can be either an instruction/operand */
typedef union Cod {
    Instruction instruction;
    Operand operand;
} Cod;


/* Register Struct: Name and id of the registers */
typedef struct Reg {
    char* name;
    int id;
} Reg;

/* Finds register info by name
 * Returns -1 if invalid */
int find_reg(char* reg_name);


/*
 * Directive:
 * Specification info for the 4 assembler directives ('.entry', '.extern', '.data', '.string')
 */
typedef struct Directive {
    char* name;  /* Can be: '.entry', '.extern', '.data', '.string' */
    int n_args;
    ArgType arg_type;
} Directive;

/* Fetches directive info by name
 * Returns NULL if invalid */
Directive* get_directive(char* directive_name);

/*
 * Op:
 * Specification info for the 16 instruction operations:
 * These are: mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rst, stop
 * Each op takes 0-2 args (operands), each of which can be used only with certain 'addressing modes'
 * (as indicated by a 1 in the relevant position of its bit array).
 */
typedef struct Op {
    char* name;  /* One of: '.entry', '.extern', '.data', '.string' */
    int opcode; /* Not necessarily unique - see funct below */
    int funct;  /* Needed to distinguish ops having the same opcode */
    int n_args;
    int first_arg_type[4]; /* a bit array indicating, which address modes can be used by the 'src' operand of this op */
    int second_arg_type[4]; /* a bit array indicating, which address modes can be used by the 'dest' operand of this op */
} Op;





char* str_info(InfoLink linker_info); /* enum to str */

/* Finds op info by name
 * Returns NULL if invalid */
Op* get_op(char* op);


/*********************************** Function Prototypes ***********************************/

/* Add a new parsed line (allocating memory if needed) */
int add_parsed_line(ParsedLine* parsed_line);

/* Free parsed_lines memory after each input file is done */
void free_parsed_lines();

/* init symbol_references array */
int init_symbol_refs();

/* init symbol_references array */
void free_symbol_refs();

/* Free all memory after each input file is done */
void free_memory();

/* to_string function for LinkerInfo enum */
char* str_info(InfoLink linker_info);

/* to_string function for DirectiveArgType enum */
char* to_str(ArgType arg_type);

/* to_string function for AddrMode enum */
char* addr_mode_str(addressing_type mode);

/* If no errors, the output files are generated */
void make_output_files(char *output_path);

/* reset the various counters before processing each file */
void count_reset();


/*********************************** External variables ***********************************/
/* Initialized in main file assembler.c, and are also used in other files */

/* Keep track of errors */
extern int errors_num;

/* Keep track of source file line_num (including blank lines) to indicate the line number in case of errors  */
extern int line_number;

/* Keep track of valid parsed lines (this is the length of the parsed_lines array below) */
extern int num_lines;

/* * Processed input lines will be stored in an array of structured data */
extern ParsedLine** parsed_lines;

/* SymbolInfos for instances of symbol references are stored in pass 1 for use in pass 2 */
extern InfoSym* ref_symbol;
extern int sym_ref_i;

#endif
