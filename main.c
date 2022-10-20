#include "utils.h"
#include "main.h"

char data_names[NUMBER_OF_DATA_NAMES][DATA_NAME_MAX_LENGTH] = {"dh", "dw", "db", "asciz", "entry", "extern"};

char command_names[NUMBER_OF_COMMAND_NAMES][COMMAND_NAME_LENGTH] = {"add","sub","and","or","nor","move", "mvhi", "mvlo", "addi", "subi",
                                                                    "andi", "ori", "nori", "bne", "beq", "blt","bgt", "lb", "sb", "lw",
                                                                    "sw", "lh", "sh", "jmp", "la", "call", "stop"};



int main(int argc, char *argv[])
{
    int i, flag;
    long ICF[2] = {0}, DCF[2] = {0};

    cmd*  cmd_head;
    cmd* cmd_list;

    data_image* data_head;
    data_image* data_list;

    symbol* symbol_head;
    symbol* symbol_list;

    if(argc == 1)
    {
        printf("missing files' name\n");
        exit(0);
    }


    for (i = 1; i < argc; i++)
    {
	cmd_head = NULL;
	data_head = NULL;
	symbol_head = NULL;

	if(!add_cmd(&cmd_head) || !add_symbol(&symbol_head) || !add_data(&data_head))
    	{
        	printf("allocation error\n");
        	exit(0);
    	}

    	cmd_list = cmd_head;                                /* sets the heads of the linked lists */
    	data_list = data_head;
    	symbol_list = symbol_head;

        flag = files(argv[i]);

        if(!flag)
        {
            printf("incorrect file name, please make sure the file's name ends with '.as'\n");
            continue;
        }

        flag = first_pass(argv[i],cmd_list,data_list,symbol_list,ICF,DCF);

        if(!flag)                                         /* some error occurred continue to the next file */
            continue;

        second_pass(argv[i],strlen(argv[i]),cmd_head,data_head,symbol_head,ICF,DCF);

        free_data(cmd_head,data_head,symbol_head);
    }

    return 1;
}

void free_data(cmd* cmd_list, data_image* data_list, symbol* symbol_list)
{
    cmd* cmd_ptr;
    data_image* data_ptr;
    symbol* symbol_ptr;

    while(cmd_list != NULL)
    {
        cmd_ptr = cmd_list->next;
        free(cmd_list);
        cmd_list = cmd_ptr;
    }

    while(data_list != NULL)
    {
        data_ptr = data_list->next;
        free(data_list);
        data_list = data_ptr;
    }

    while(symbol_list != NULL)
    {
        symbol_ptr = symbol_list->next;
        free(symbol_list);
        symbol_list = symbol_ptr;
    }
}

int files(char* filename)
{
    int i, correct_name = 1, flag = 0, cnt = 1;


    for(i = 0; i < strlen(filename); i++)
    {
        if(flag)
        {   /* checks if the ending of the file's name is correct */
            if((cnt == 1 && filename[i] != 'a') || (cnt == 2 && filename[i] != 's') || (cnt > 2 && filename[i] != ' '))    
                correct_name = 0;

            cnt++;
        }
        if(filename[i] == '.')
        {   /* if there is 2 or more dots in the file's name, reset all the variables and continue the checking process */
            if(flag)                    
            {
                correct_name = 1;
                cnt = 1;
            }
            flag = 1;
        }
    }

    if(!correct_name || cnt < 3)
        return 0;

    return 1;
}
