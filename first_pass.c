#include "utils.h"
#include "first_pass.h"
#include "input_functions.h"

int first_pass(char* filename, cmd* cmd_list, data_image* data_list, symbol* symbol_list, long* ICF,long* DCF)
{
    FILE *fp;
    char str[LINE_SIZE] = {0};
    long DC[2] = {0};
    int flag, line = 0, correct = TRUE;
    cmd* cmd_head = cmd_list;
    symbol* symbol_head = symbol_list;
    DC[0] = 4;
    cmd_list->type = -1;    		  /* if there is no command lines, the type will stay -1 and will be used as a sign to skip the printing of the command lines */

    fp = fopen (filename, "r");

    if(fp == NULL)
    {
        printf("error: the file is inaccessible. file's name: %s\n",filename);
        return 0;
    }


        while(!feof(fp))
        {
            line++;
            if(fgets(str, LINE_SIZE, fp) == NULL)
            {
                if(line == 1)
                {
                    printf("error: couldn't read the file. file's name: %s , line's number: %d\n", filename, line);
                    return 0;
                }

		else
                	break;
            }

            flag = cmd_check(cmd_list,data_list,symbol_list,str,filename,line,DC);

            if(!flag)                                                           /* checks if error occurred while parsing the data */
                correct = FALSE;
        }
        fclose(fp);

        while(cmd_list->next != NULL)                                            /* reaching the last node to save the last address */
            cmd_list = cmd_list->next;

        ICF[0] = cmd_list->IC;                                                   /* sets the total addresses */
        DCF[0] = DC[0];

        while(data_list != NULL)
        {
            data_list->DC += ICF[0];                                             /* updates the addresses of the data */
            data_list = data_list->next;
        }
        while(symbol_list != NULL)
        {
            if(symbol_list->type == DATA && !symbol_list->is_entry && !symbol_list->is_external)
                symbol_list->address+=ICF[0];

            symbol_list = symbol_list->next;
        }
        if(!symbols_multiplicity_check(symbol_head,filename))                     /* checks if error occurred while parsing the data */
            return 0;

        if(!update_labels(cmd_head,symbol_head,filename))
            return 0;

        if(!update_codes(cmd_head))
            return 0;

        if(cmd_list == cmd_head && cmd_list->type == -1)                           /* no command lines */
            cmd_list->type = -1;

    return correct;
}

int cmd_check(cmd* cmd_list, data_image* data_list, symbol* symbol_list, char* str, char* filename, int line, long *DC)
{
    char name[LABEL_LENGTH] = {0}, name1[LABEL_LENGTH] = {0};
    int j, type, IC = 100, is_symbol = 0, flag = 0, flag1 = 0, is_data = 0, name_length = 0, name_digit = 0, i = 0;
    unsigned long num; /* num is unsigned long so it will be able to distinguish between error to correct number */
                       /* correct number's range is signed 16 bits, long is at least 16 bits in any machine */
                       /* so the error value will be USHRT_MAX - 65535 - requires unsigned 16 bits */

    while(isspace(str[i]))
        i++;

    if(str[i] == ';')                   				          /* command line */
    {
        i++;

        while(isspace(str[i]))
            i++;

        if(str[i] != '\0' )
        {
            printf("error: extraneous text. file's name: %s , line's number: %d\n", filename, line);
            return 0;
        }
        return 1;
    }

    if(str[i] == '\0')                					  	  /* empty line */
        return 1;

    if(isdigit(str[i]))
    {
        printf("error: command's name has to start with a letter.  file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }
    if(str[i] == '.')                                    			 /* case data */
    {
        i++;
        is_data = 1;
    }

    while(isalpha(str[i]))                               			 /* saves the command's\symbol's name */
    {
        if(name_length >= LABEL_LENGTH-1)                			 /* subtract '\0' from the macro's value */
        {
            flag = TRUE;
            break;
        }
        name[name_length] = str[i];
        i++;
        name_length++;
    }

    while(isdigit(str[i]))			                                 /* checks if the name includes a digit */
    {
        if(name_length >= LABEL_LENGTH-1)
        {
            flag = TRUE;
            break;
        }
        name_digit = TRUE;
        name[name_length] = str[i];
        i++;
        name_length++;
    }

    if(!isalpha(str[i]) && !isdigit(str[i]) && !isspace(str[i]) && str[i] != ':' && str[i] != '\0')
    {
        printf("error: label's or command's name cannot contain punctuation characters. file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }

    type = name_check(name,name_length);

    if(type == -1)                                     			       /* undefined command name, checks if symbol */
    {
            if(flag)
            {
                printf("error: label's name cannot contain more than 31 characters. file's name: %s , line's number: %d\n", filename, line);
                return 0;
            }
            if(str[i] == ' ' || str[i] == '\0')
            {
                while(isspace(str[i]))
                    i++;

                if(str[i] == ':')
                {
                    printf("error: cannot be a space between the symbol's name and ':'. file's name: %s , line's number: %d\n", filename, line);
                    return 0;
                }
                else
                {
                    printf("error: undefined command's name. file's name: %s , line's number: %d\n", filename, line);
                    return 0;
                }
            }
            if(str[i++] == ':')                                    	      /* case symbol */   /* skips ':' */
                is_symbol = TRUE;


            name_length = 0;

            j = i;

            while(isspace(str[j]))
                j++;

            if(str[j] == '\0')
            {
                printf("error: missing command. file's name: %s , line's number: %d\n", filename, line);
                return 0;
            }

            if(str[i] != ' ')                                   
            {
                printf("error: no space between the symbol's name and the command's name. file's name: %s , line's number: %d\n", filename, line);
                return 0;
            }

            while(isspace(str[i]))
                i++;

            if(str[i] == '.')                                    	     /* case data */
            {
                i++;
                is_data = TRUE;
            }
            if(!isalpha(str[i]))
            {
                printf("error: command's name has to start with a letter.  file's name: %s , line's number: %d\n", filename, line);
                return 0;
            }
            while(isalpha(str[i]))                             		    /* saves the command's name */
            {
                name1[name_length] = str[i];
                i++;
                name_length++;
            }

            type = name_check(name1,name_length);

            if(type == -1)
            {
                printf("error: undefined command name. file's name: %s , line's number: %d\n", filename, line);
                return 0;
            }
    }

    if(name_digit && !is_symbol)
    {
        printf("error: command's name can't includes a digit.  file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }
    if(str[i] == ':' && type != -1)
    {
        printf("error: symbol's name is a 'safe word'.  file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }

    if(type == STOP)
    {
        while(isspace(str[i]))
            i++;

        CHECK_EXTRANEOUS_TEXT_B(str,i)

        if(!cmd_list->first_time)                                          /* checks if the node is already in use */
        {
            if(!add_cmd(&cmd_list))
            {
                printf("allocation error\n");
                exit(0);
            }


            while(cmd_list->next != NULL)                                  /* continue to the new node */
            {
                IC+=4;
                cmd_list = cmd_list->next;
            }

            cmd_list->IC = IC;                                             /* updates the address */
        }
        cmd_list->first_time = FALSE;                                      /* marks the second time - from the second time creates a new node for each data */
        cmd_list->type = type;

        if(is_symbol)
        {
            if(!symbol_list->first_time)                                   /* checks if the node is already in use */
            {
                if(!add_symbol(&symbol_list))
                {
                    printf("allocation error\n");
                    exit(0);
                }
                while(symbol_list->next != NULL)                           /* continue to the new node */
                    symbol_list = symbol_list->next;
            }
            symbol_list->first_time = FALSE;                               /* marks the second time - from the second time creates a new node for each data */

            if(!label_check(cmd_list,symbol_list,str,filename,line,SYMBOL))
                return 0;

            symbol_list->type = CMD;
            symbol_list->address = cmd_list->IC;
        }
        return 1;
    }

    j = i;

    while(isspace(str[j]))
        j++;

    if(str[j] == '\0')
    {
        if(is_data)
            printf("error: missing data. file's name: %s , line's number: %d\n", filename, line);
        else
            printf("error: missing operands. file's name: %s , line's number: %d\n", filename, line);

        return 0;
    }

    if(str[i++] != ' ')
    {
        printf("error: no space between the command's name and the first operand. file's name: %s , line's number: %d\n", filename, line);
        return 0;
    }

    if(type >= ADDITION)                                                    /* case data */
    {
        if(type < ENTRY+ADDITION)
        {
            if(is_symbol)                                                   /* checks if the node is already in use */
            {
                if(!symbol_list->first_time)
                {
                    if(!add_symbol(&symbol_list))
                    {
                        printf("allocation error\n");
                        exit(0);
                    }
                    while(symbol_list->next != NULL)                         /* continue to the new node */
                        symbol_list = symbol_list->next;
                }
                symbol_list->first_time = FALSE;                             /* marks the second time - from the second time creates a new node for each data */
                symbol_list->address = DC[0];

                if(!label_check(cmd_list,symbol_list,str,filename,line,SYMBOL))
                    return 0;
            }
            if(!data_list->first_time)                                       /* checks if the node is already in use */
            {
                if(!add_data(&data_list))
                {
                    printf("allocation error\n");
                    exit(0);
                }
                while(data_list->next != NULL)                               /* continue to the new node */
                    data_list = data_list->next;
            }

            data_list->first_time = FALSE;                                   /* marks the second time - from the second time creates a new node for each data */
            data_list->type = type;

            if (type < NUMBER_OF_DATA + ADDITION)                            /* dh,db,dw */
            {
                data_list->DC = DC[0];                                       /* updates the address */
                is_data = data_number_check(data_list, str + i, filename, line, DC);
            }

            else if (type == ASCIZ + ADDITION)                               /* asciz */
            {
                data_list->DC = DC[0];
                is_data = data_string_check(data_list, str + i, filename, line, DC);
            }
        }
        else                                                                 /* entry,extern */
        {
            if(is_symbol)
            {
                printf("warning: symbol shouldn't represent a variable (entry,extern). file's name: %s , line's number: %d\n", filename, line);
                return 0;
            }
            if(!symbol_list->first_time)                                     /* checks if the node is already in use */
            {
                if(!add_symbol(&symbol_list))
                {
                    printf("allocation error\n");
                    exit(0);
                }
                while(symbol_list->next != NULL)                             /* continue to the new node */
                    symbol_list = symbol_list->next;
            }
            symbol_list->first_time = FALSE;                                 /* marks the second time - from the second time creates a new node for each data */

            if(type == ENTRY+ADDITION)
                symbol_list->is_entry = TRUE;
            else
            {
                symbol_list->is_external = TRUE;
                symbol_list->address = 0;
            }

            is_data = label_check(cmd_list,symbol_list,str+i,filename,line,SYMBOL);
        }
    }

    if(type >= ADDITION && !is_data)                                          /* error occurred while parsing the data */
        return 0;

    if(type >= ADDITION+ENTRY && is_data)                                     /* ENTRY - EXTERN */
    {
        while(isspace(str[i]))
            i++;

        while(isalpha(str[i]))                                                /* skips the label's name */
            i++;

        CHECK_EXTRANEOUS_TEXT_B(str,i)
        return 1;                                                             /* finished correctly */
    }
    else if(is_data)                                                          /* dh,dw,db,asciz finished correctly */
        return 1;

    if(!cmd_list->first_time)                                                 /* checks if the node is already in use */
    {
        if(!add_cmd(&cmd_list))
        {
            printf("allocation error\n");
            exit(0);
        }


        while(cmd_list->next != NULL)                                         /* continue to the new node */
        {
            IC+=4;
            cmd_list = cmd_list->next;
        }

        cmd_list->IC = IC;                                                    /* updates the address */
    }
    cmd_list->first_time = FALSE;                                             /* marks the second time - from the second time creates a new node for each data */
    cmd_list->type = type;

    if(is_symbol)							      /* symbol */
    {
        if(!symbol_list->first_time)                                          /* checks if the node is already in use */
        {
            if(!add_symbol(&symbol_list))
            {
                printf("allocation error\n");
                exit(0);
            }
            while(symbol_list->next != NULL)                                  /* continue to the new node */
                symbol_list = symbol_list->next;
        }
        symbol_list->first_time = FALSE;                                      /* marks the second time - from the second time creates a new node for each data */

        if(!label_check(cmd_list,symbol_list,str,filename,line,SYMBOL))
            return 0;

        symbol_list->type = CMD;
        symbol_list->address = cmd_list->IC;
    }

    if(type > NUMBER_OF_I_CMD_C)                                               /* all J cmd */
    {
        flag1 = check_j_cmd(cmd_list,str+i,filename,line);

        if(flag1 == USHRT_MAX)
            return 0;

        return 1;
    }


    num = reg_check(str+i,filename,line);

    while(isspace(str[i]))
        i++;

    i++;                                                                        /* skips '$' */
    if(num == USHRT_MAX)
        return 0;

    if(type < NUMBER_OF_R_CMD_A)                                                /* add - nor */
        cmd_list->op.r.rs = (int)num;

    else if(type < NUMBER_OF_R_CMD_B)                                           /* move - mvlo */
        cmd_list->op.r.rs = (int)num;

    else if(type <= NUMBER_OF_I_CMD_C)                                          /* all I cmd */
        cmd_list->op.i.rs = (int)num;

    SKIP_THE_NUMBER(str,i)

    CHECK_COMMA(str,i)
										/* all R cmd && bne - bgt */
    if(type < NUMBER_OF_R_CMD_B || (type >= NUMBER_OF_I_CMD_A && type < NUMBER_OF_I_CMD_B))    
        num = reg_check(str+i,filename,line);
    else
        num = num_check(str+i,filename,line);
    i++;                                                                        /* skips '$' */

    if(num == USHRT_MAX)
        return 0;

    if(type < NUMBER_OF_R_CMD_A)                                                /* add - nor */
        cmd_list->op.r.rt = (int)num;

    else if(type < NUMBER_OF_R_CMD_B)                                           /* move - mvlo */
        cmd_list->op.r.rd = (int)num;

    else if(type < NUMBER_OF_I_CMD_A)                                           /* addi - nori */
        cmd_list->op.i.immed = (int)num;

    else if(type < NUMBER_OF_I_CMD_B)                                           /* bne - bgt */
        cmd_list->op.i.rt = (int)num;

    else
        cmd_list->op.i.immed = (int)num;                                        /* lb - sh */

    SKIP_THE_NUMBER(str,i)

    if(type >= NUMBER_OF_R_CMD_A  && type < NUMBER_OF_R_CMD_B)                  /* move - mvlo */
    {
        CHECK_EXTRANEOUS_TEXT_B(str,i)
        return 1;                                                               /* finished correctly */
    }


    CHECK_COMMA(str,i)

    else if(type >= NUMBER_OF_I_CMD_A && type < NUMBER_OF_I_CMD_B)              /* bne - bgt */
    {
        flag1 = label_check(cmd_list,symbol_list,str+i,filename,line,CMD);
        if(flag1 == USHRT_MAX)              /* checcckkkkkkkkkkkkk*/
            return 0;
        else
            flag1 = 1;
    }

    else
    {
        num = reg_check(str+i,filename,line);
        i++;
    }

    if(num == USHRT_MAX)
        return 0;

    if(!flag1)                                                                  /* no label - skips the number and updates the new data */
    {
        if(type < NUMBER_OF_R_CMD_A)                                            /* add - nor */
            cmd_list->op.r.rd = (int)num;
										/* addi - nori ||  lb - sh */
        else if((type >= NUMBER_OF_R_CMD_B && type < NUMBER_OF_I_CMD_A) || type >= NUMBER_OF_I_CMD_B) 
            cmd_list->op.i.rt = (int)num;

        SKIP_THE_NUMBER(str,i)				
    }
    else
        while(isalpha(str[i]))                                                  /* is label - skips the label's name */
            i++;

    while(isspace(str[i]))
        i++;

    CHECK_EXTRANEOUS_TEXT_B(str,i)

    return 1;                                                                   /* finished correctly */
}


