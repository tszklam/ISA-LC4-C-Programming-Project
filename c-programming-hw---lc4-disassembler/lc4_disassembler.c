/************************************************************************/
/* File Name : lc4_disassembler.c 										*/
/* Purpose   : This file implements the reverse assembler 				*/
/*             for LC4 assembly.  It will be called by main()			*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lc4_hash.h"

#define MAX_LEN (256)
#define INSN_11_9(I) (((I) >> 9) & 0x7)
#define INSN_8_6(I) (((I) >> 6) & 0x7)
#define INSN_5_3(I) (((I) >> 3) & 0x7)
#define INSN_2_0(I) ((I) & 0x7)
#define INSN_IMM(I) (((I) >> 5) & 0x1)

/*
 *sign extend
 */
signed short int SextoI(unsigned short int ins, int length) 
{
    signed short int sign, final;
    int mask;
    unsigned short int x ;
    x = ins & ((1 << length) - 1);
    sign = (x >> (length - 1)) & 0x0001;
    if(sign == 0) // positive
    { 
        return x;
    } 
    else // negative
    { 
        mask = (1 << (length - 1));
        final = (x & ~mask) - (mask);
        return final;
    }
}

int reverse_assemble (lc4_memory_segmented* memory) 
{
	int i = 0;
	row_of_memory *arith_memory, *logic_memory;
	char buff[MAX_LEN] = {0};
	for(i=0;i<memory->num_of_buckets;i++)
	{
		arith_memory = search_opcode(memory->buckets[i], 0x0001);//opcode 0001
		logic_memory = search_opcode(memory->buckets[i], 0x0005);//opcode 0101
		while(arith_memory != NULL || logic_memory != NULL)
		{
			if(arith_memory != NULL)
			{
				memset(buff,0,MAX_LEN);
				if(INSN_IMM(arith_memory->contents))
			    {
			        // printf("Add immediate\n");
			        sprintf(buff, "ADD R%d, R%d, #%d",INSN_11_9(arith_memory->contents),INSN_8_6(arith_memory->contents), SextoI(arith_memory->contents, 5));
			    }
			    else
			    {
			        switch(INSN_5_3(arith_memory->contents))
			        {
			            case 0://add
			                // printf("Add\n");
			                sprintf(buff, "ADD R%d, R%d, R%d",INSN_11_9(arith_memory->contents),INSN_8_6(arith_memory->contents), INSN_2_0(arith_memory->contents));
			                break;
			            case 1://mul
			                // printf("Mul\n");
			                sprintf(buff, "MUL R%d, R%d, R%d",INSN_11_9(arith_memory->contents),INSN_8_6(arith_memory->contents), INSN_2_0(arith_memory->contents));
			 
			                break;
			            case 2://sub
			                // printf("Sub\n");
			                sprintf(buff, "SUB R%d, R%d, R%d",INSN_11_9(arith_memory->contents),INSN_8_6(arith_memory->contents), INSN_2_0(arith_memory->contents));
			                break;
			            case 3://div
			                // printf("Div\n");              
			            	sprintf(buff, "DIV R%d, R%d, R%d",INSN_11_9(arith_memory->contents),INSN_8_6(arith_memory->contents), INSN_2_0(arith_memory->contents));
			                break;
			            default:
			                printf("INVALID INSTRUCTION\n");
			                return -1;
			        }
			    }
			    arith_memory->assembly = (char*) malloc(sizeof(char)*(strlen(buff)+1));
			    strcpy(arith_memory->assembly, buff);

			}
			if(logic_memory != NULL)
			{
				memset(buff,0,MAX_LEN);
				if(INSN_IMM(logic_memory->contents))
			    {
			        // printf("And immediate\n");
			        sprintf(buff, "AND R%d, R%d, #%d",INSN_11_9(logic_memory->contents),INSN_8_6(logic_memory->contents), SextoI(logic_memory->contents, 5));
			    }
			    else
			    {
			        switch(INSN_5_3(logic_memory->contents))
			        {
			            case 0://and
			                printf("And\n");
			                sprintf(buff, "AND R%d, R%d, R%d",INSN_11_9(logic_memory->contents),INSN_8_6(logic_memory->contents), INSN_2_0(logic_memory->contents));
			                break;
			            case 1://not
			                // printf("Not\n");
			                sprintf(buff, "NOT R%d, R%d",INSN_11_9(logic_memory->contents),INSN_8_6(logic_memory->contents));
			                break;
			            case 2://or
			                // printf("Or\n");
			                sprintf(buff, "OR R%d, R%d, R%d",INSN_11_9(logic_memory->contents),INSN_8_6(logic_memory->contents), INSN_2_0(logic_memory->contents));
			                break;
			            case 3://xor
			                // printf("Xor\n");              
			            	sprintf(buff, "XOR R%d, R%d, R%d",INSN_11_9(logic_memory->contents),INSN_8_6(logic_memory->contents), INSN_2_0(logic_memory->contents));
			                break;
			            default:
			                printf("INVALID INSTRUCTION\n");
			                return -1;
			        }
			    }
			    logic_memory->assembly = (char*) malloc(sizeof(char)*(strlen(buff)+1));
			    strcpy(logic_memory->assembly, buff);
			}
			arith_memory = search_opcode(memory->buckets[i], 0x0001);//opcode 0001
			logic_memory = search_opcode(memory->buckets[i], 0x0005);//opcode 0101
		}
	}

	return 0 ;
}
