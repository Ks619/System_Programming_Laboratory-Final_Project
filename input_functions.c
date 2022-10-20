#include "utils.h"
#include "first_pass.h"


int data_string_check(data_image* data_list, char* str, char* filename, int line, long* DC)
{
    int i = 0, j = 0;
    char s[MAX_CHARACTERS_IN_STRING] = {0};

    while(isspace(str[i]))
        i++;

    if(str[i++] != '\"')
    {
        while(isspace(str[i]))
            i++;

        if(str[i] == '\"')
            printf("error: undefined text before the quotation mark. file's name: %s , line's number: %d\n", filename, line);
        else
            printf("error: missing quotation mark. file's name: %s , line's number: %d\n", filename, line);

        return 0;
    }
    while(str[i] != '\"' && str[i] != '\0')                                   /* saves the string */
    {
        if(j>=MAX_CHARACTERS_IN_STRING-1)                           	      /* subtracts the null '\0' from the macro */
        {
            printf("error: string cannot contain more than 11 letters. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        DC[0]++;
        s[j] = str[i];
        i++;
        j++;
    }
    DC[0]++;                                                                 /* counts '\0' */

    if(str[i] == '\0')
    {
        printf("error: missing quotation mark. file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }
    if(str[i] != '\"')
    {
        printf("error: string must contain only letters. file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }

    while(isspace(str[++i]));

    if(str[i] == '\0')
    {
        strcpy(data_list->str,s);                                	    /* saves the string */
        return 1;                                                	    /* finished correctly */
    }

    printf("error: extraneous text. file's name: %s , line's number: %d\n", filename, line);
    return 0;

}

int label_check(cmd* cmd_list, symbol* symbol_list, char* str, char* filename, int line, int type)
{
    char name[LABEL_LENGTH] = {0};
    int flag, i = 0,name_length = 0;

    while(isspace(str[i]))
        i++;
    if(str[i] == '$')
    {
        printf("error: register instead of label.  file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }
    if (isdigit(str[i]))
    {
        printf("error: label's name can't start with a digit.  file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }
    if(!isalpha(str[i]))
    {
        printf("error: label's name has to start with a letter.  file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }
    while(isalpha(str[i]))                               /* saves the label's name */
    {
        if(i == LABEL_LENGTH)
        {
            printf("error: label's name can't be longer than 31 characters.  file's name: %s , line's number: %d\n", filename, line);
            return USHRT_MAX;
        }
        name[name_length] = str[i];
        i++;
        name_length++;
    }
    flag = name_check(name,name_length);

    if(flag>=0)
    {
        printf("error: label's name is a 'safe word'. file's name: %s , line's number: %d\n",filename, line);
        return USHRT_MAX;
    }

    if(type == SYMBOL)                                   /* saves the label's name */
    {
        strcpy(symbol_list->name,name);
        symbol_list->type = DATA;
    }
    else
        strcpy(cmd_list->op.i.label,name);

    return 1;
}

int name_check(char* name, int length)
{
    int i;

    for(i = 0; i < NUMBER_OF_COMMAND_NAMES; i++)        /* checks if the name is "safe word" */
    {
        if(length != strlen(command_names[i]) && length != strlen(data_names[i]))
            continue;

        if(memcmp(name,command_names[i],length) == 0)
            return i;

        if(i < NUMBER_OF_DATA_NAMES)
            if(memcmp(name,data_names[i],length) == 0)
                return i+ADDITION;                     /* in case of data, add addition to 'type', so we can distinguish between the data types and the cmd types */
    }
    return -1;
}

long reg_check(char* str,char* filename, int line)
{
    int num, i = 0;
    char* ptr;

    while(isspace(str[i]))
        i++;
    if(isdigit(str[i]))
    {
        printf("error: missing operand. file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }
    if(str[i++] != '$')
    {
        while(isspace(str[i]))
            i++;

        if(str[i] == '$')
            printf("error: undefined text before the register. file's name: %s , line's number: %d\n", filename, line);
        else
        {
            i++;
            while(isspace(str[i]))
                i++;

            while(isalpha(str[i]))
                i++;

            while(isspace(str[i]))
                i++;

            if(str[i] == '$')
                printf("error: undefined text before the register. file's name: %s , line's number: %d\n", filename, line);
            else
                printf("error: missing operand. file's name: %s , line's number: %d\n", filename, line);
        }
        return USHRT_MAX;
    }

    if(str[i] == ' ')
    {
        printf("error: cannot be space between the operand and the number. file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }
    num = strtol(str + i,&ptr,DECIMAL);                                   /* extracts the number from the string */

    VALID_NUM(str,i)

    if(num < 0 || num > 31)                                               /*  register range */
    {
        printf("error: invalid number - not in range.  file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }

    CHECK_EXTRANEOUS_TEXT_A(ptr,i)
}

long num_check(char* str,char* filename, int line)
{
    int num, i = 0;
    char* ptr;

    while(isspace(str[i]))
        i++;

    if(!isdigit(str[i]) && str[i] != '-' && str[i] !='+')
    {
        printf("error: undefined text before the number. file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;
    }
    num = strtol(str + i,&ptr,DECIMAL);                                 /* extracts the number from the string */

    VALID_NUM(str,i)

    if(num > MAX_INT_2_BYTES || num < MIN_INT_2_BYTES)
    {
        printf("error: invalid number - not in range.  file's name: %s , line's number: %d\n", filename, line);
        return USHRT_MAX;;
    }
    CHECK_EXTRANEOUS_TEXT_A(ptr,i)
}

long check_j_cmd(cmd* cmd_list,char* str,char* filename, int line)
{
    char name[LABEL_LENGTH] = {0};
    int num, i = 0,name_length = 0;

    while(isspace(str[i]))
        i++;

    if(str[i] != '$' && !isalpha(str[i]) && !isdigit(str[i]))
    {
        printf("error: label's name has to with a letter and register has to start with '$'.  file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }
    if(str[i] == '$')
    {
        num = reg_check(str+i,filename,line);

        if(num == USHRT_MAX)
            return 0;

        i++;                                                                  /* skips '$' */
        cmd_list->op.j.reg = 1;                                               /* case jmp */
        cmd_list->op.j.address = num;
    }
    else
    {
        if(isdigit(str[i]))
        {
            printf("error: missing operand. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        while(isalpha(str[i]))                                                /* saves the label's name */
        {
            name[name_length] = str[i];
            i++;
            name_length++;
        }

        if(name_check(name,name_length) >= 0)
        {
            printf("error: label's name is a 'safe word'. file's name: %s , line's number: %d\n",filename, line);
            return 0;
        }
        strcpy(cmd_list->op.j.label,name);
        cmd_list->op.j.reg = 0;
    }

    while(isspace(str[i]))
        i++;

    SKIP_THE_NUMBER(str,i)
    CHECK_EXTRANEOUS_TEXT_B(str,i)

    return 1;
}

int data_number_check(data_image* data_list,char* str,char* filename,int line, long* DC)
{
    int i = 0;
    long num = 1;
    char* ptr;

    while(num != 0)
    {
        num = strtol(str + i,&ptr,DECIMAL);                             /* extracts the number from the string */

        if(errno == ERANGE)                                             /* checks if some error occurred */
        {
            printf("error: the number you entered require more than 4 bytes. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        if(str[i] == ',')
        {
            printf("error: unnecessary comma. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        if(str == ptr)
        {
            printf("error: not a number. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        str = ptr;                                                       /* sets the pointer to point at the end of the number in the string */
        i = 0;                                                           /* sets the index */


        if(num != (long int)num)
        {
            printf("error: invalid set member - not an integer. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        switch(data_list->type)                                          /* checks if the number in range */
        {
            case DH+ADDITION:
                if(num < MIN_INT_2_BYTES || num > MAX_INT_2_BYTES)
                {
                    printf("error: the number %d require more than 2 bytes. file's name: %s , line's number: %d\n", (int)num, filename, line);
                    return 0;
                }
                DC[0]+=2;
                break;

            case DW+ADDITION:
                if(num <= MIN_INT_4_BYTES || num > MAX_INT_4_BYTES)
                {
                    printf("error: the number %d require more than 4 bytes. file's name: %s , line's number: %d\n", (int)num, filename, line);
                    return 0;
                }
                DC[0]+=4;
                break;

            case DB+ADDITION:
                if(num < MIN_INT_1_BYTES || num > MAX_INT_1_BYTES)
                {
                    printf("error: the number %d require more than 1 bytes. file's name: %s , line's number: %d\n", (int)num, filename, line);
                    return 0;
                }
                DC[0]+=1;
                break;

            default:
                printf("ERROR - DEFAULT SWITCH");
                break;
        }
        data_list->arr[data_list->array_length++] = num;

        while (isspace(str[i]))
            i++;

        if(str[i] == '\0')                                 /* finished correctly */
            return 1;

        if(isdigit(str[i]))
        {
            printf("error: missing comma after the number. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }

        if(str[i++] != ',')
        {
            printf("error: not a number. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }

        while (isspace(str[i]))
            i++;

        if(str[i] == '\0')
        {
            printf("error: extraneous comma. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
    }
    return 1;
}

int symbols_multiplicity_check(symbol* symbol_list, char* filename)
{
    int flag = TRUE;
    symbol* ptr = symbol_list->next;                                                         /* sets ptr to point at the next node of symbol_list */
    symbol* symbol_head = symbol_list;

    while(symbol_list != NULL)
    {
            while(ptr != NULL)
            {										                                    /* checks if they both point at the same node */
                if(!symbol_list->checked && strlen(symbol_list->name) == strlen(ptr->name) && symbol_list != ptr)
                {
                    if(strcmp(symbol_list->name,ptr->name) == 0)
                    {									     /* checks multiple declaration */
                        if((symbol_list->is_external && ptr->is_external) || (symbol_list->is_entry && ptr->is_entry))   
                        {
                             printf("warning: multiple declaration of the variable: %s. file's name: %s. \n",symbol_list->name,filename);
                             ptr->checked = 1;
                        }
											    /* checks if the variable declared as external and entry  */
                        if((symbol_list->is_external && ptr->is_entry) || (symbol_list->is_entry && ptr->is_external))          
                        {
                            printf("error: variables can't be declared in both type external and entry. file's name: %s. \n",filename);
                            flag = FALSE;
                            ptr->checked = 1;
                        }
                       if((!(symbol_list->is_external) && !(symbol_list->is_entry)) && (!(ptr->is_external) && !(ptr->is_entry)))
                       {
                            printf("error: the symbol: %s , is already in use. file's name: %s. \n",symbol_list->name,filename);
                            flag = FALSE;
                            ptr->checked = 1;
                       }
                    }
                }
                ptr = ptr->next;
            }
        symbol_list = symbol_list->next;
        ptr = symbol_head;
    }
    return flag;
}

int update_labels(cmd* cmd_list, symbol* symbol_head, char* filename)
{
    long int address = 1;                                                                                            /* also represent the address */

    while(cmd_list !=NULL)
    {
        if(cmd_list->type >= NUMBER_OF_I_CMD_A && cmd_list->type < NUMBER_OF_I_CMD_B)                                /* bne - bgt  */
        {
            address = find_address_of_label(symbol_head,cmd_list->op.i.label);

            if(address == -1)
            {
                printf("couldn't find the source of the label: %s. file's name: %s. \n", cmd_list->op.i.label,filename);
                return 0;
            }
            if(address != 0)                                                                                         /* only if the label isn't extern */
                address -= cmd_list->IC;

            if(address < MIN_INT_2_BYTES || address > MAX_INT_2_BYTES)
            {
                printf("error: the range between the addresses requires more than 16 bits. label: %s, file's name: %s. \n", cmd_list->op.i.label,filename);
                return 0;
            }

            cmd_list->op.i.immed = address;
        }
        else if(cmd_list->type > NUMBER_OF_I_CMD_C && cmd_list->type != STOP)                                        /* all j cmd */
        {
             if(!(cmd_list->op.j.reg))
                 address = find_address_of_label(symbol_head,cmd_list->op.j.label);

             if(address == -1)
             {
                 printf("couldn't find the source of the label: %s. file's name: %s. \n", cmd_list->op.j.label,filename);
                 return 0;
             }
            cmd_list->op.j.address = address;

        }
        cmd_list = cmd_list->next;
    }
    return 1;
}

long int find_address_of_label(symbol* symbol_list, char* label_name)
{
    while(symbol_list !=NULL)
    {
        if(strlen(symbol_list->name) == strlen(label_name))
            if(strcmp(symbol_list->name,label_name) == 0)/* compares between the labels */
            {
                if(symbol_list->is_external)
                    return 0;
                                                     /* there is 2 nodes for entry symbols, 1 - represent the command, 2 - represent that the symbol type is entry */
                if(symbol_list->is_entry)            /* this node doesn't have the address, continue to the node which contain the address */
                {
                    symbol_list = symbol_list->next;
                    continue;
                }

                else
                {
                    if(symbol_list->type == DATA)
                        return (symbol_list->address);
                    else
                        return symbol_list->address;
                }
            }
        symbol_list = symbol_list->next;
    }
    return -1;
}

int update_codes(cmd* cmd_list)
{
    while(cmd_list != NULL)
    {
        switch(cmd_list->type)
        {
            case ADD:
                cmd_list->op.r.opcode = 0;
                cmd_list->op.r.funct = 1;
                break;

            case SUB:
                cmd_list->op.r.opcode = 0;
                cmd_list->op.r.funct = 2;
                break;

            case AND:
                cmd_list->op.r.opcode = 0;
                cmd_list->op.r.funct = 3;
                break;

            case OR:
                cmd_list->op.r.opcode = 0;
                cmd_list->op.r.funct = 4;
                break;

            case NOR:
                cmd_list->op.r.opcode = 0;
                cmd_list->op.r.funct = 5;
                break;

            case MOVE:
                cmd_list->op.r.opcode = 1;
                cmd_list->op.r.funct = 1;
                break;

            case MVHI:
                cmd_list->op.r.opcode = 1;
                cmd_list->op.r.funct = 2;
                break;

            case MVLO:
                cmd_list->op.r.opcode = 1;
                cmd_list->op.r.funct = 3;
                break;

            case ADDI:
                cmd_list->op.i.opcode = 10;
                break;

            case SUBI:
                cmd_list->op.i.opcode = 11;
                break;

            case ANDI:
                cmd_list->op.i.opcode = 12;
                break;

            case ORI:
                cmd_list->op.i.opcode = 13;
                break;

            case NORI:
                cmd_list->op.i.opcode = 14;
                break;

            case BNE:
                cmd_list->op.i.opcode = 15;
                break;

            case BEQ:
                cmd_list->op.i.opcode = 16;
                break;

            case BLT:
                cmd_list->op.i.opcode = 17;
                break;

            case BGT:
                cmd_list->op.i.opcode = 18;
                break;

            case LB:
                cmd_list->op.i.opcode = 19;
                break;

            case SB:
                cmd_list->op.i.opcode = 20;
                break;

            case LW:
                cmd_list->op.i.opcode = 21;
                break;

            case SW:
                cmd_list->op.i.opcode = 22;
                break;

            case LH:
                cmd_list->op.i.opcode = 23;
                break;

            case SH:
                cmd_list->op.i.opcode = 24;
                break;

             case JMP:
                cmd_list->op.j.opcode = 30;
                    break;

            case LA:
                cmd_list->op.j.opcode = 31;
                break;

            case CALL:
                cmd_list->op.j.opcode = 32;
                break;

            case STOP:
                cmd_list->op.j.opcode = 63;
                break;

            default:
                break;
        }
        cmd_list = cmd_list->next;
    }
    return 1;
}
