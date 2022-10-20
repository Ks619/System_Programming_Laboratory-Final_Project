#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define LABEL_LENGTH 32                       /* label's length = 31 + '\0' */
#define COMMAND_NAME_LENGTH 5
#define DATA_NAME_MAX_LENGTH 7
#define NUMBER_OF_DATA_NAMES 6
#define NUMBER_OF_COMMAND_NAMES 27
#define MAX_NUMBERS_IN_ARRAY 40               /* each line contains 80 characters and between each number there needs to be a comma */
#define MAX_CHARACTERS_IN_STRING 12           /* 11 characters + '\0' */

enum {FALSE,TRUE};

/* data image - linked_list */
typedef struct data_image* dptr;

typedef struct data_image {

    int first_time;
    int type;
    long int arr[MAX_NUMBERS_IN_ARRAY];
    int array_length;
    char str[MAX_CHARACTERS_IN_STRING];
    long DC;
    dptr next;

}data_image;

/* symbols - linked_list */
typedef struct symbol* sptr;

typedef struct symbol {

    int first_time;
    char name[LABEL_LENGTH];
    long address;
    int type;                                   /* 1 - data, 2 - code */
    int is_external;
    int is_entry;
    int checked;                                /* marks a symbol that already checked, while checking multiplicity of symbols */
    sptr next;

}symbol;

typedef struct cmd_i{

    unsigned int opcode: 6;
    unsigned int rs: 5;
    unsigned int rt: 5;
    signed int immed: 16;
    char label[LABEL_LENGTH];
}cmd_i;

typedef struct cmd_j{

    unsigned int opcode: 6;
    unsigned int reg: 1;
    signed int address: 25;
    char label[LABEL_LENGTH];
}cmd_j;

typedef struct cmd_r{

    unsigned int opcode: 6;
    unsigned int rs: 5;
    unsigned int rt: 5;
    unsigned int rd: 5;
    unsigned int funct: 5;


}cmd_r;

typedef union ops {
        cmd_r r;
        cmd_i i;
        cmd_j j;
}ops;


/* command lines , linked list */
typedef struct cmd* cmd_ptr;

typedef struct cmd{

    int first_time;
    long IC;
    int type;
    union ops op;
    cmd_ptr next;
}cmd;


