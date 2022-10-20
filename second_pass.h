
#define ADDITION            30                       /* in case of data, adds addition to 'type', so we can distinguish between the data types and the cmd types */
#define NUMBER_OF_DATA      3                        /* dh,dw,db */
#define NUMBER_OF_R_CMD_B   8			     /* all R commands */
#define NUMBER_OF_I_CMD_A   13                       /* addi - nori <-> 9 - 13 */
#define NUMBER_OF_I_CMD_B   17                       /* bne - bgt   <-> 14 - 17 */
#define NUMBER_OF_I_CMD_C   22			     /* lb - sh   <-> 18 - 22 */

enum data_types{DH,DW,DB};

/* this function prints the R,I,J commands in the requested format */
/* the function receives the file's pointer, the command's linked list, ICF, DCF */
void print_byteByByte(FILE*,cmd*,long*,long*);

/* this function prints the data image in the requested format */
/* the function receives the file's pointer, the command's linked list, the data image - linked list, ICF, DCF */
void print_data_image(FILE*,cmd*,data_image*,long*);

/* this function prints the extern symbols in the requested format if there's any */
/* the function receives the file's name (file.ext), the command's linked list, and the symbol's linked list */
void print_extern_symbol(char*,cmd*,symbol*);

/* this function prints the entry symbols in the requested format if there's any */
/* the function receives the file's name (file.ent), the command's linked list, and the symbol's linked list */
void print_entry_symbol(char*,symbol*);

/* there are 2 nodes of entry symbols, one of them represents the cmd with the address, and the second indicates that the symbol is entry */
/* this function marks the command-symbol as an entry symbol */
/* the function receives the symbol's linked list */
void set_up_entry_symbol(symbol*);
