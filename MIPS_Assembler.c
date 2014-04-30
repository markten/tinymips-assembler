#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define BUFF_SIZE 64
#define ADDR_SIZE 5
#define CONST_SIZE 16
#define INSTR_SIZE 32
#define NUM_R_TYPE 5
#define NUM_I_TYPE 4
#define R_TYPE 1
#define I_TYPE 2
#define J_TYPE 3

#define ADD     0 
#define SUB     1
#define SLT     2
#define AND     3
#define OR      4

#define ADDI    0
#define BEQ     1
#define LB      2
#define SB      3

char *conv_to_bin(int value);
char *parse_element(char* element, int instr_type);
char *get_r_funct(int funct);
char *zero_pad(char *pad_string, char *cat_string, int total_len);
char *one_pad(char *pad_string, char *cat_string, int total_len);

int debug = 0;

char *zero_pad(char *pad_string, char *cat_string, int total_len)
{
    for(int zero_count = 0; zero_count < (total_len - sprintf(cat_string, "%s", cat_string)); zero_count++)
        strcat(pad_string, "0");
}

char *one_pad(char *pad_string, char *cat_string, int total_len)
{
    for(int zero_count = 0; zero_count < (total_len - sprintf(cat_string, "%s", cat_string)); zero_count++)
        strcat(pad_string, "1");
}

char *parse_element(char* element, int instr_type)
{
	int valuea, valueb, valuec;
    valuea = valueb = valuec = INT_MAX;
    int offset = INT_MAX;

    char *offset_string;
    char *value_string;
    char *final_string = (char *)malloc((CONST_SIZE+ADDR_SIZE)*sizeof(char));
    char *addr_string = (char *)malloc(CONST_SIZE*sizeof(char));
    char *temp;
    addr_string[0] = final_string[0] = '\0';

	if(debug) printf("\tParsing: %s\n", element);
	sscanf(element, "%d($%d)", &offset, &valuea);
    sscanf(element, "$s%d", &valueb);
    sscanf(element, "%d", &valuec);
    if(debug)printf("\tvaluea %d, valueb %d, offset %d\n", valuea, valueb, offset);

    // Handle a program address w/ offset
    if((offset != INT_MAX) && (valuea != INT_MAX))
    {
        
        if(final_string != NULL)
        {
            if(debug) printf("\tElement is an address w/ offset\n");
            if(offset < pow(2,16)) offset_string = conv_to_bin(offset);
            else {printf("ERROR: Invalid value for offset [%d]!\n", offset); return NULL;}
            if(valuea < 32) value_string = conv_to_bin(valuea);
            else {printf("ERROR: Invalid value for regaddr [%d]!\n", valuea); return NULL;}
            if(debug) printf("\tSize value_string: %d\n", sprintf(value_string, "%s", value_string));
            zero_pad(final_string, value_string, ADDR_SIZE);
            strcat(final_string, value_string);
            if(debug) printf("\tSize offset_string: %d\n", sprintf(offset_string, "%s", offset_string));
            if(instr_type == R_TYPE) zero_pad(final_string, offset_string, ADDR_SIZE);
            if(instr_type == I_TYPE) zero_pad(final_string, offset_string, CONST_SIZE);
            strcat(final_string, offset_string);
            if(debug) printf("\tFinal String: %s\n", final_string);
            if((value_string != NULL) && (offset_string != NULL)) {free(offset_string); free(value_string);}
            return final_string;
        }
        else return NULL;
    }
    
    // Handle a register address
	else if(valueb < pow(2, ADDR_SIZE))
	{
        if(debug) printf("\tElement [%d] is an address.\n", valueb);		
        temp = conv_to_bin(valueb);
        zero_pad(addr_string, temp, ADDR_SIZE);
        strcat(addr_string, temp);
        if(temp != NULL) free(temp);
        if(debug) printf("\taddr_string: %s\n", addr_string);
        return addr_string;
	}
    
    // Handle a constant
    else if(valuec < pow(2, CONST_SIZE - 1))
    {
        if(debug) printf("\tElement is a numerical value.\n");
        temp = conv_to_bin(valuec);
        if(valuec >= 0) zero_pad(addr_string, temp, CONST_SIZE);
        else one_pad(addr_string, temp, CONST_SIZE);
        strcat(addr_string, temp);
        if(temp != NULL) free(temp);
        if(debug) printf("\taddr_string: %s\n", addr_string);
        return addr_string;
    }
    
    // Handle bad stuff
	else
    {
        if(strcmp(element, "") == 0) return NULL;
        else {printf("ERROR: Invalid element [%s]\n", element); return NULL;}
    }
}

char *conv_to_bin(int value)
{
    //value = abs(value);    
    int string_size;
    int index;
    char *string_in_binary;

    if(value  < 0)
    {
        value = abs(value) - 1;
        if(value != 0)
        {
            string_size = ceil(log2(value));
            if((ceil(log2(value)) == log2(value)) || value == 1) string_size++;
        }
        else string_size = 1;
        if(debug) printf("\tstring_size: %d\n", string_size);
        string_in_binary = (char *)malloc((string_size+1)*sizeof(char));  
        if(debug) printf("\tZero the string\n"); 
        for(index = 0; index < string_size; index++)
            string_in_binary[index] = '1';
        string_in_binary[string_size] = '\0';
        if(debug) printf("\tSet bits: %s\n", string_in_binary);
    	for(index = string_size; index > 0; index--)
    		if(value >= pow(2,index))	{value -= pow(2,index); string_in_binary[string_size-index-1] = '0';}
        if(value > 0) string_in_binary[string_size-1] = '1';
    	if(debug) printf("\tBinary Address: %s\n", string_in_binary);
    	return string_in_binary;
    }

    if(value >= 0)
    {
        if(value != 0)
        {
            string_size = ceil(log2(value));
            if((ceil(log2(value)) == log2(value)) || value == 1) string_size++;
        }
        else string_size = 1;
        if(debug) printf("\tstring_size: %d\n", string_size);
        string_in_binary = (char *)malloc((string_size+1)*sizeof(char));  
        if(debug) printf("\tZero the string\n"); 
        for(index = 0; index < string_size; index++)
            string_in_binary[index] = '0';
        string_in_binary[string_size] = '\0';
        if(debug) printf("\tSet bits: %s\n", string_in_binary);
    	for(index = string_size; index > 0; index--)
    		if(value >= pow(2,index))	{value -= pow(2,index); string_in_binary[string_size-index-1] = '1';}
        if(value > 0) string_in_binary[string_size-1] = '1';
    	if(debug) printf("\tBinary Address: %s\n", string_in_binary);
    	return string_in_binary;
    }
}

char *get_r_funct(int funct)
{

    switch(funct)
    {
        case ADD:   return "100000";    break;
        case SUB:   return "100010";    break;
        case SLT:   return "101010";    break;
        case AND:   return "100100";    break;
        case OR:    return "100101";    break;
        default:    return NULL;        break;
    }

}

int main(int argc, char* argv[])
{

	int index, adj_index;
	int instruction_type, funct;

	char buffer[BUFF_SIZE];
//	char *filename;
	char instr[32]; instr[0] = '\0';
	char strings[5][20]; for(int index = 0; index < 5; index++) strings[index][0] = '\0';
	char addr[5];
	char *token;
	char *lasts;
	FILE *source;
    FILE *drain;

	// keywords
	const char *rtype_instr[] = {"add", "sub", "slt", "and", "or"};
	const char *itype_instr[] = {"addi", "beq", "lb", "sb"};
    const int rtype_order[] = {2,3,1};
    const int itype_ordera[] = {2,1,2};
    const int itype_orderb[] = {2,1,3};
	
	// Open file provided as argument
	printf("Opening File: %s\n", argv[1]);
	source = fopen(argv[1], "r");
    drain = fopen(argv[2], "w");
    fprintf(drain, "%s", "### Generated by some unctious, messy code :)\n\n");
	
	if(source != NULL)
	{
		while(fgets(buffer, BUFF_SIZE, source) != NULL)
		{
			
			// Init vars
            instr[0] = '\0';
            for(int index = 0; index < 5; index++) strings[index][0] = '\0';
            index = instruction_type = funct = 0;
            char *element;

			// Tokenize the line
			printf("\nReading line: %s", buffer);			

			token = strtok(buffer, " \t\n\r,");
			//printf("Cutting rest of string\n");
	
			while(token != NULL)
			{
				//printf("\tCopying token (%s)...\n", token);
				strcpy(strings[index], token);				
				token = strtok(NULL, " \t\n\r,");
				index++;
			}
			
			//printf("Done cutting string.\n");


			/*** Interpret the line ***/
			// Check for R-Type Instruction
			for(int r_index = 0; r_index < NUM_R_TYPE; r_index++)
			{
				//printf("Checking <%s> against <%s>...\n", strings[0], rtype_instr[index]);
				if(strcmp(strings[0], rtype_instr[r_index]) == 0)
				{
					instruction_type = R_TYPE;
					funct = r_index;
					break;
				}
			}
			
			// Check for I-Type Instruction
			if(instruction_type == 0)
			{
				//printf("Instruction not R Type\n");
				for(int i_index = 0; i_index < NUM_I_TYPE; i_index++)
				{
				//printf("Checking <%s> against <%s>...\n", strings[0], itype_instr[index]);
					if(strcmp(strings[0], itype_instr[i_index]) == 0)
					{
						instruction_type = I_TYPE;
                        funct = i_index;
						break;
					}
				}
			}

			// Check for J-Type Instruction
			if(instruction_type == 0)
			{
				if(strcmp(strings[0], "j") == 0) instruction_type = J_TYPE;
			}
			
            // Build Instruction
			switch(instruction_type)
			{
				case R_TYPE:
				    printf("\tThe instruction was an R-Type.\n");
    				strcpy(instr, "000000");
                    
                    for(int cpy_index = 0; cpy_index < 3; cpy_index++)
                    {
                        element = parse_element(strings[rtype_order[cpy_index]], R_TYPE);         
                        if(element != NULL)
                        {
                            if(debug) printf("\tElement: %s\n", element);
                            strcat(instr, element);
                            if(cpy_index == 2) zero_pad(instr, element, ADDR_SIZE*2);
                            if(element != NULL) free(element); element = NULL;
                        }
                    }
    				strcat(instr, get_r_funct(funct));
				break;

				case I_TYPE:
	    			printf("\tThe instruction was an I-Type.\n");
                    if(funct == ADDI) strcpy(instr, "001000");
                    else if (funct == BEQ) strcpy(instr, "000100");
                    else if (funct == LB) strcpy(instr, "100000");
                    else if (funct == SB) strcpy(instr, "101000");
                    else printf("ERROR: Unknown I-Type Function!\n");
                    for(int cpy_index = 0; cpy_index < 3; cpy_index++)
                    {
                        if((funct == LB) || (funct == SB)) element = parse_element(strings[itype_ordera[cpy_index]], I_TYPE);
                        else element = parse_element(strings[itype_orderb[cpy_index]], I_TYPE);         
                        if(element != NULL)
                        {                             
                            if((funct == LB) || (funct == SB) && (cpy_index == 0)) strncat(instr, element, ADDR_SIZE);
                            else if((funct == LB) || (funct == SB) && (cpy_index == 2))
                            {
                                zero_pad(instr, &element[ADDR_SIZE], ADDR_SIZE);
                                if(debug) printf("Elementbeforepaste: %s\n", element);
                                strcat(instr, &element[ADDR_SIZE]);
                            }
                            else strcat(instr, element);
                            if(element != NULL) free(element); element = NULL;
                        }
                    }
				break;

				case J_TYPE:
	    			printf("\tThe instruction was a J-Type.\n");
                    strcpy(instr, "0000100000000000");
                    for(int cpy_index = 1; cpy_index < 5; cpy_index++)
                    {
                        element = parse_element(strings[cpy_index], J_TYPE);
                        if(element != NULL)
                        {
                            if(debug) printf("\tElement: %s\n", element);
                            strcat(instr, element);
                            if(element != NULL) free(element); element = NULL;
                        }
                    }
                    
				break;

				default:
				printf("The instruction was an invalid type.\n");
				break;
			}
            
            // Write data to .DAT file
            printf("\tinstr: %s\n", instr);               
            for(int thirtytwo = 0; thirtytwo < 32; thirtytwo++)
            {            
                if(thirtytwo % 8 == 0) fprintf(drain, "%c", ' ');
                fprintf(drain, "%c", instr[thirtytwo]);
            }
            fprintf(drain, "\n");
		}
	}

	else printf("Unable to open file :(\n");
	// Close the file
	fclose(source);
    fclose(drain);
	//getchar();
	
	return 0;
}
