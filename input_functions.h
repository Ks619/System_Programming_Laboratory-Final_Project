/* this function checks if the string in azciz's command is legal */
/* the function receives the data image - linked list, the current file's name, line, string-input, and the DC */
int data_string_check(data_image*,char*,char*,int,long*);

/* this function checks if J's command is legal */
/* the function receives the command's linked list, the current file's name, line, and the string-input */
long check_j_cmd(cmd*,char*,char*,int);

/* this function sets the opcode and the funct of the commands */
/* the function receives the command's linked list*/
int update_codes(cmd*);

/* this function sets all the addresses which connect to a label, in the commands -> bne - bgt , and all J's command */
/* the function receives the command's linked list, the symbol's linked list and the current file's name */
int update_labels(cmd*,symbol*,char*);

/* this function checks if there is multiplicity declarations of symbol's */
/* the function receives the symbol's linked list and the current file's name */
int symbols_multiplicity_check(symbol*,char*);

/* this function checks if the label's\symbol's name is legal */
/* the function receives the command's linked list, the symbol's linked list, the current file's name, line and the label's type CODE\DATA */
int label_check(cmd*,symbol*,char*,char*,int,int);

/* this function checks if the data in the commands dh,dw,db is legal */
/* the function receives the data image - linked list, the symbol's linked list, the current file's name, line, string-input, and the DC */
int data_number_check(data_image*,char*,char*,int,long*);

/* this function manages all the parser functions, calls the right functions depeneding the command's type */
/* the function receives the command's linked list, the data image - linked list, the symbol's linked list, the current file's name, line, string-input, and the DC */
int cmd_check(cmd*,data_image*,symbol*,char*,char*,int,long*);

/* this function checks if the register is legal */
/* the function receives the current file's name, line and the string-input */
long reg_check(char*,char*,int);

/* this function checks if the number which belongs to addi - nori commands is legal */
/* the function receives the current file's name, line and the string-input */
long num_check(char*,char*,int);

/* this function adds a new node to the data image - linked list */
/* the function receives the data image - linked list head */
int add_data(data_image**);

/* this function adds a new node to the command's linked list */
/* the function receives the command's linked list head */
int add_cmd(cmd**);

/* this function adds a new node to the symbol's linked list */
/* the function receives the symbol's linked list head */
int add_symbol(symbol**);


