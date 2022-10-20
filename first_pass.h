#include <errno.h>
#include <limits.h>

enum data_types{DH,DW, DB, ASCIZ, ENTRY};
enum {SYMBOL,DATA,CMD};
enum {ADD,SUB,AND,OR,NOR,MOVE,MVHI,MVLO,ADDI,SUBI,ANDI,ORI,NORI,BNE,BEQ,BLT,BGT,LB,SB,LW,SW,LH,SH,JMP,LA,CALL,STOP};

#define LINE_SIZE           81                     /* each line contains 80 characters + '\n'  */
#define NUMBER_OF_DATA      3                      /* dh,dw,db */
#define NUMBER_OF_R_CMD_A   5                      /* add - nor   <-> 0 - 4 */
#define NUMBER_OF_R_CMD_B   8                      /* move - mvlo <-> 5 - 8 */
#define NUMBER_OF_I_CMD_A   13                     /* addi - nori <-> 9 - 13 */
#define NUMBER_OF_I_CMD_B   17                     /* bne - bgt   <-> 14 - 17 */
#define NUMBER_OF_I_CMD_C   22                     /* lb - sh   <-> 18 - 22 */
#define DECIMAL             10
#define ADDITION            30                     /* in case of data, adds addition to 'type', so we can distinguish between the data types and the cmd types */
#define MAX_INT_1_BYTES     127
#define MIN_INT_1_BYTES    (-128)
#define MAX_INT_2_BYTES     32767
#define MIN_INT_2_BYTES    (-32768)
#define MIN_INT_4_BYTES    (-2147483647)
#define MAX_INT_4_BYTES     2147483647

/* this macro checks if there is extraneous text */
#define CHECK_EXTRANEOUS_TEXT_A(ptr,i)  i = 0; \
                                        while(isspace(*((ptr)+(i))))                                      /* skips the space after the register */ \
                                        (i)++;   \
                                        if(*((ptr)+(i)) == ',' || *((ptr)+(i)) == '\0')                                                                     \
                                        return num;                                                                                                \
                                        if(*((ptr)+(i)) == '$')                                                                                    \
                                        printf("error: missing comma. file's name: %s , line's number: %d\n", filename, line);                     \
                                        else                                                                                                       \
                                        printf("error: undefined text after the number. file's name: %s , line's number: %d\n", filename, line);  \
                                        return USHRT_MAX;

/* this macro checks if there is extraneous text */
#define CHECK_EXTRANEOUS_TEXT_B(str,i)   if(*((str)+(i)) == ',') \
                               {                         \
                               (i)++;                      \
                               while(isspace(*((str)+(i))))    \
                               (i)++;                      \
                               if(*((str)+(i)) == '$')         \
                               printf("error: extraneous operand. file's name: %s , line's number: %d\n", filename, line); \
                               else                      \
                               printf("error: extraneous comma. file's name: %s , line's number: %d\n", filename, line);   \
                               return 0;                 \
                               }                         \
                               while(isspace(*((str)+(i))))    \
                               (i)++;                      \
                               if(*((str)+(i)) != '\0')        \
                               {                         \
                               printf("error: extraneous text. file's name: %s , line's number: %d\n", filename, line);    \
                               return 0;               \
                               }

/* this macro checks if the comma's format is legal */
#define CHECK_COMMA(str,i)    if(*((str)+(i)) != ',') \
                              { \
                              printf("error: missing comma. file's name: %s , line's number: %d\n", filename, line); \
                              return 0;                                                                          \
                              }                   \
                              (i)++;                 \
                              while (isspace(*((str)+(i))))                                                            \
                              (i)++;                                                                               \
                              if(*((str)+(i)) == ',')                                                                  \
                              { \
                              printf("error: extraneous comma. file's name: %s , line's number: %d\n", filename, line); \
                              return 0;                                                                          \
                              }

/* this macro skips the number in the string after parsering it */
#define SKIP_THE_NUMBER(str,i) while (isdigit(*((str)+(i)))) \
                                    (i)++; \
                                    while(isspace(*((str)+(i)))) \
                                    (i)++;

/* this macro checks if a number in the commands dh,dw,db is legal */
#define VALID_NUM(str,i) { if(!isdigit(*((str)+(i))) && num == 0)                                                                     \
                         {                                                                                                    \
                            printf("error: after '$' should be a number. file's name: %s , line's number: %d\n", filename, line); \
                            return USHRT_MAX;                                                                                         \
                         }                                                                                                    \
                         if(num != (int)num)                                                                                  \
                         {                                                                                                    \
                         printf("error: invalid set member - not an integer. file's name: %s , line's number: %d\n", filename, line);\
                         return USHRT_MAX;                                                                                                \
                         }}

/* this function checks if the command's name is legal or if the symbol's\label's name is a 'safe word' */
/* the function receives command's\label's name and the name's length */
int name_check(char*,int);

/* this function searches and returns the label's address (IC) */
/* the function receives the symbol's linked list, and the label's name */
long int find_address_of_label(symbol*,char*);



extern char data_names[NUMBER_OF_DATA_NAMES][DATA_NAME_MAX_LENGTH];

extern char command_names[NUMBER_OF_COMMAND_NAMES][COMMAND_NAME_LENGTH];


