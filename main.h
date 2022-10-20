/* this function free all the linked lists */
/* the function receives the heads of all the linked lists */
void free_data(cmd*,data_image*,symbol*);

/* the first pass parsing the input and checks if errors occurred in the input */
/* the function receives the current file's name, the heads of the command's linked list, data image - linked list and the symbol's linked list, and ICF, DCF */
int first_pass(char*,cmd*,data_image*,symbol*,long*,long*);

/* the second pass prints all the data */
/* the function receives the file's name, the name's length, the heads of the command's linked list, data image - linked list and the symbol's linked list,
and ICF, DCF */
void second_pass(char*,int,cmd*,data_image*,symbol*,long*,long*);

/* this function checks if the file's name is legal */
/* the function receives the file's name */
int files(char*);

/* this function adds a new node to the data image - linked list */
/* the function receives the data image - linked list head */
int add_data(data_image**);

/* this function adds a new node to the command's linked list */
/* the function receives the command's linked list head */
int add_cmd(cmd**);

/* this function adds a new node to the symbol's linked list */
/* the function receives the symbol's linked list head */
int add_symbol(symbol**);
