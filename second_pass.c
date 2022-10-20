#include "utils.h"
#include "second_pass.h"

void second_pass(char* filename, int name_length ,cmd* cmd_head, data_image* data_head, symbol* symbol_head, long* ICF, long* DCF)
{
    FILE *fp_ob;
    char* ob;                    
    char* ent;
    char* ext;
    int i = 0;
   		       /* the ending length of the file is 2, and the max ending length of the file to be created is 3 */
    ob = (char*)calloc(name_length+1,sizeof(char));
    ent = (char*)calloc(name_length+1,sizeof(char));
    ext = (char*)calloc(name_length+1,sizeof(char));

    if(ob == NULL || ent == NULL || ext == NULL)
    {
     	printf("allocation error\n");
	exit(0);
    }

    while(filename[i] != '.')
    {
        ob[i] = filename[i];
        i++;
    }
    strcpy(ext,ob);
    strcpy(ent,ob);
    strcat(ob,".ob");
    strcat(ext,".ext");
    strcat(ent,".ent");

    fp_ob = fopen (ob, "w");

    set_up_entry_symbol(symbol_head);

    if(cmd_head->type != -1)                                                        /* checks if there is any command lines */
        print_byteByByte(fp_ob,cmd_head,ICF,DCF);

    if(data_head->arr[0] != '\0' || data_head->str[0] != '\0')                      /* checks if there is any data */
        print_data_image(fp_ob,cmd_head,data_head,ICF);

    print_entry_symbol(ent,symbol_head);
    print_extern_symbol(ext,cmd_head,symbol_head);


   fclose(fp_ob);
   free(ob);
   free(ent);
   free(ext);
}

void print_byteByByte(FILE* fp_ob, cmd* cmd_list, long* ICF, long* DCF)
{
    /* builds new structures for the printing process,
     * each structure contain the values of the command to be printed,
     * and contains a new structure which allows as to treat the union's variables in different bits order */

    union unitedR
    {
        long savePlace;                             /* sets the union's size to be sizeof(long) */
        struct Rvalues
        {
            int zero: 6;
            unsigned int funct: 5;
            unsigned int rd: 5;
            unsigned int rt: 5;
            unsigned int rs: 5;
            unsigned int opcode: 6;
        } values;

        struct forPrint
        {
            unsigned char one;
            unsigned char two;
            unsigned char three;
            unsigned char four;
        } forPrintR;
    } R;

    union unitedI
    {
        long savePlace;
        struct Ivalues
        {
            int immed: 16;
            unsigned int rt: 5;
            unsigned int rs: 5;
            unsigned int opcode: 6;
        } values;

        struct forPrint forPrintI;
    } I;

    union unitedJ
    {
        long savePlace;
        struct Jvalues
        {
            int immed: 25;
            unsigned int reg: 1;
            unsigned int opcode: 6;
        } values;

        struct forPrint forPrintJ;
    } J;



    fprintf(fp_ob, "     %ld %ld\n",(ICF[0]-100+4),DCF[0]-4);

    while(cmd_list != NULL)
    {
	fprintf(fp_ob, "%04ld ",cmd_list->IC);					

        if (cmd_list->type < NUMBER_OF_R_CMD_B)                         /* all R cmd */
        {
            R.values.opcode = cmd_list->op.r.opcode;                    /* sets the new struct with the right values */
            R.values.rs = cmd_list->op.r.rs;
            R.values.rt = cmd_list->op.r.rt;
            R.values.rd = cmd_list->op.r.rd;
            R.values.funct = cmd_list->op.r.funct;
            R.values.zero = 0;

            fprintf(fp_ob,"%02X ", R.forPrintR.one);                    /* prints the values in the requested format, while using the 'forPrint' structure */
            fprintf(fp_ob,"%02X ", R.forPrintR.two);                    
            fprintf(fp_ob,"%02X ", R.forPrintR.three);
            fprintf(fp_ob,"%02X", R.forPrintR.four);
        }

        else if (cmd_list->type <= NUMBER_OF_I_CMD_C)                   /* all I cmd */
        {
            I.values.opcode = cmd_list->op.i.opcode;
            I.values.rs = cmd_list->op.i.rs;
            I.values.rt = cmd_list->op.i.rt;
            I.values.immed = cmd_list->op.i.immed;

            fprintf(fp_ob,"%02X ", I.forPrintI.one);
            fprintf(fp_ob,"%02X ", I.forPrintI.two);
            fprintf(fp_ob,"%02X ", I.forPrintI.three);
            fprintf(fp_ob,"%02X", I.forPrintI.four);
        }

        else                                                             /* all J cmd */
        {
            J.values.opcode = cmd_list->op.j.opcode;
            J.values.reg = cmd_list->op.j.reg;
            J.values.immed = cmd_list->op.j.address;

            fprintf(fp_ob,"%02X ", J.forPrintJ.one);
            fprintf(fp_ob,"%02X ", J.forPrintJ.two);
            fprintf(fp_ob,"%02X ", J.forPrintJ.three);
            fprintf(fp_ob,"%02X", J.forPrintJ.four);
        }
        fprintf(fp_ob,"\n");
        cmd_list = cmd_list->next;
    }
}
void print_data_image(FILE* fp_ob,cmd* cmd_list, data_image* data_list, long* ICF)
{
    int cnt, i = 0, count = 0, first_time = 0;

    while (data_list != NULL)
    {
        if(data_list->type < NUMBER_OF_DATA+ADDITION)                                    /* dh,dw,db */
        {
            if(!first_time)
            {
                first_time = 1;
                if(cmd_list->type != -1)                                                /* there is at least 1 command line, skips its address */
                    ICF[0]+=4;
		
                fprintf(fp_ob, "%04ld ",ICF[0]);
            }

            while (data_list->arr[i] != '\0')
            {										/* checks if a new line is necessary */
                if (count == 4 && (i < data_list->array_length-1 || data_list->next != NULL || cnt != 0))
                {
                    ICF[0]+=4;
                    count = 0;
		            fprintf(fp_ob, "\n%04ld ",ICF[0]);
                }

                if(data_list->type == DH + ADDITION)                                    /* case dh */
                {
                    cnt = 2;
                    while(cnt--)
                    {
                        fprintf(fp_ob, "%02X ", (char)data_list->arr[i] & 0xff);
                        data_list->arr[i] >>= 8;                                        /* moves 8 bits to the right for the next printing */
                        count++;
											/* checks if a new line is necessary */
                        if (count == 4 && (i < data_list->array_length-1 || data_list->next != NULL || cnt != 0))
                        {
                            ICF[0]+=4;
                            count = 0;
			                fprintf(fp_ob, "\n%04ld ",ICF[0]);
                        }
                    }
                }
                else if(data_list->type == DW + ADDITION)                               /* case dw */
                {
                    cnt = 4;
                    while(cnt--)
                    {
                        fprintf(fp_ob, "%02X ", (char)data_list->arr[i] & 0xff);
                        data_list->arr[i] >>= 8;                                       /* moves 8 bits to the right for the next printing */
                        count++;
										       /* checks if a new line is necessary */
                        if (count == 4 && (i < data_list->array_length-1 || data_list->next != NULL || cnt != 0))
                        {
                            ICF[0]+=4;
                            count = 0;
                            
			    fprintf(fp_ob, "\n%04ld ",ICF[0]);
                        }
                    }
                }

                else                                                                   /* case db */
                {
                    fprintf(fp_ob, "%02X ", (char)data_list->arr[i] & 0xff);
                    count++;
                }
                i++;
            }
        }

        else if(data_list->type == NUMBER_OF_DATA+ADDITION)                           /* asciz */
        {
            if(!first_time)
            {
                first_time = 1;
                if(cmd_list->type != -1)                                              /* there is at least 1 command line, skips its address */
                    ICF[0]+=4;

                fprintf(fp_ob, "%04ld ",ICF[0]);
            }

            while (data_list->str[i] != '\0')                                         /* checks if a new line is necessary */
            {
                if (count == 4)
                {
                    ICF[0]+=4;
                    count = 0;
                    
		    fprintf(fp_ob, "\n%04ld ",ICF[0]);
                }

                fprintf(fp_ob, "%02X ", data_list->str[i]);
                count++;
                i++;
            }

            if (count == 4)                                                          /* checks if a new line is necessary */
            {
                ICF[0]+=4;
                count = 0;
                
		fprintf(fp_ob, "\n%04ld ",ICF[0]);
            }
            fprintf(fp_ob, "%02X ", data_list->str[i]);
            count++;
        }

        i = 0;                                                                      /* resets the index and moves to the next node */
        data_list = data_list->next;
    }
}

void print_extern_symbol(char* ext, cmd* cmd_list, symbol* symbol_list)
{
    symbol *symbol_head = symbol_list;
    FILE* fp_ext;
    int external_exists = 0;

    while (cmd_list != NULL)
    {										    /* bne - bgt */
        if (cmd_list->type >= NUMBER_OF_I_CMD_A &&cmd_list->type <NUMBER_OF_I_CMD_B)                                      
        {
            while (symbol_list != NULL)
            {
                if (symbol_list->is_external)
                    if (strlen(symbol_list->name) == strlen(cmd_list->op.i.label))
                        if (strcmp(symbol_list->name, cmd_list->op.i.label) ==0)    /* compares between the symbols */
                        {
			    if(!external_exists)
			    {
				external_exists = 1;
    				fp_ext = fopen (ext, "w");
			    }

                            fprintf(fp_ext, "%s %04ld \n", cmd_list->op.i.label, cmd_list->IC);
                            break;
                        }

                symbol_list = symbol_list->next;
            }
        }

        else if (cmd_list->type >NUMBER_OF_I_CMD_C)                                  /* all j cmd */
        {
            while (symbol_list != NULL)
            {
                if (symbol_list->is_external)
                    if (strlen(symbol_list->name) == strlen(cmd_list->op.j.label))
                        if (strcmp(symbol_list->name, cmd_list->op.j.label) ==0)    /* compares between the symbols */
                        {
			    if(!external_exists)
			    {
				external_exists = 1;
    				fp_ext = fopen (ext, "w");
			    }

                            fprintf(fp_ext, "%s %04ld \n", cmd_list->op.j.label, cmd_list->IC);
                            break;
                        }

                symbol_list = symbol_list->next;
            }
        }
        symbol_list = symbol_head;
        cmd_list = cmd_list->next;
    }

    if(external_exists)
	fclose(fp_ext);
    
}

void print_entry_symbol(char* ent, symbol* symbol_list)
{
    FILE* fp_ent;
    int entry_exists = 0;
    
    while(symbol_list != NULL)
    {
        if (symbol_list->is_entry && symbol_list->address != 0)
        {
            if(!entry_exists)
    	    {
		entry_exists = 1;
        	fp_ent = fopen (ent, "w");
            }
	    fprintf(fp_ent, "%s %04ld \n", symbol_list->name, symbol_list->address);
	}

        symbol_list = symbol_list->next;
    }
    if(entry_exists)
    	fclose(fp_ent);
}

void set_up_entry_symbol(symbol* symbol_list)
{
    char symbol_name[LABEL_LENGTH] = {0};
    int flag = 0;
    symbol* symbol_head = symbol_list;
    symbol* symbol_keep = symbol_list;

    while(symbol_list != NULL)
    {
        if(symbol_list->is_entry && !flag && !symbol_list->address)       /* checks if the symbol is an entry */
        {
            strcpy(symbol_name, symbol_list->name);                       /* saves the symbol */
            symbol_keep = symbol_list;
            symbol_list = symbol_head;
            flag = 1;                                                     /* flag is a sign to start searching the cmd-symbol and set it as an entry */
        }
        else if(strlen(symbol_list->name) == strlen(symbol_name) && strcmp(symbol_list->name, symbol_name) == 0 && flag && !symbol_list->is_entry) 
        {								  /* compares between the symbols */
            symbol_list->is_entry = 1;                                    /* marks the cmd-symbol as an entry symbol */
            flag = 0;
            symbol_list = symbol_keep;
        }

        symbol_list = symbol_list->next;
    }
}


