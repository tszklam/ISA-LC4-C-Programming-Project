/***************************************************************************
 * file name   : assembler.c                                               *
 * author      : tjf & you                                                 *
 * description : This program will assemble a .ASM file into a .OBJ file   *
 *               This program will use the "asm_parser" library to achieve *
 *			     its functionality.										   * 
 *                                                                         *
 ***************************************************************************
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "asm_parser.h"

int main(int argc, char** argv) {

	char* filename = NULL ;					// name of ASM file
	char  program [ROWS][COLS] = {0}; 			// ASM file line-by-line
	char  program_bin_str [ROWS][17] = {0}; 		// instructions converted to a binary string
	unsigned short int program_bin [ROWS] = {0} ; // instructions in binary (HEX)
    int i = 0;

    //check argc 
    if(argc < 2)
    {
        printf("error1: usage: ./assembler <assembly_file.asm>\n");
        return 1;
    }
    filename = argv[1];
    // printf("argc = %d, argv[1] = %s , filename = %s\n", argc, argv[1], filename);
    read_asm_file(filename, program);
    for(i=0; i<ROWS;i++)
    {
        if(strlen(program[i])!=0)
        {
            parse_instruction(&program[i][0], &program_bin_str[i][0]);
            //printf("res = %s\n",program_bin_str[i]);
            program_bin[i] = str_to_bin(&program_bin_str[i][0]);
            // printf("hex = 0x%X\n",program_bin[i]);
        }
    }

    write_obj_file(filename, program_bin);
}
