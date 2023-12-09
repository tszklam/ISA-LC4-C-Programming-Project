/************************************************************************/
/* File Name : lc4_loader.c		 										*/
/* Purpose   : This file implements the loader (ld) from PennSim		*/
/*             It will be called by main()								*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"


//Endianness
unsigned short int big_to_lil(unsigned short int x) {
  short int upper, lower, final;
  upper = (x & 0xFF00) >> 8; 
  lower = (x & 0x00FF) << 8; 
  final = upper | lower;
  return final;
}
/* declarations of functions that must defined in lc4_loader.c */

FILE* open_file(char* file_name)
{
	FILE *fp = NULL;
  	fp = fopen(file_name,"rb");
	  if(fp == NULL)
	  {
	    printf("error3:  file %s not exist\n", file_name);
	    return NULL;
	  }
	return fp ;
}

int parse_file (FILE* my_obj_file, lc4_memory_segmented* memory)
{
  unsigned short int buff, addr,n,words;
  unsigned int file_length_bytes;
  unsigned short memoryAddress;
  int i = 0, j = 0;
  int ret = 0;
  row_of_memory *tmp_memory;
  while(fread(&buff, sizeof(unsigned short int), 1, my_obj_file) == 1) 
	{ 
		// look for headers
		if(buff == 0xDECA) //code setion
		{
			//get init address
			fread(&addr, sizeof(unsigned short int), 1, my_obj_file);
			memoryAddress = big_to_lil(addr);
			//get number
			fread(&n, sizeof(unsigned short int), 1, my_obj_file);
			for (i = 0; i < big_to_lil(n); i++) {
				fread(&words, sizeof(unsigned short int), 1, my_obj_file);
				tmp_memory = search_tbl_by_address(memory, memoryAddress+i);
				//search address if exist update it, if not add new entry
				if(tmp_memory == NULL)
					add_entry_to_tbl(memory, memoryAddress+i, big_to_lil(words));
				else
					tmp_memory->contents = big_to_lil(words);
			}
		} 
		else if(buff == 0xDADA) // data setion
		{ 
			// get init address
			fread(&addr, sizeof(unsigned short int), 1, my_obj_file);
			memoryAddress = big_to_lil(addr);
			// get number
			fread(&n, sizeof(unsigned short int), 1, my_obj_file);
			for (i = 0; i < big_to_lil(n); i++) 
			{
				fread(&words, sizeof(unsigned short int), 1, my_obj_file);
				tmp_memory = search_tbl_by_address(memory, memoryAddress+i);
				//search address if exist update it, if not add new entry
				if(tmp_memory == NULL)
					add_entry_to_tbl(memory, memoryAddress+i, big_to_lil(words));
				else
					tmp_memory->contents = big_to_lil(words);
			}
		}
		else if(buff == 0xB7C3) //Symbol
		{
			// get init address
			fread(&addr, sizeof(unsigned short int), 1, my_obj_file);
			memoryAddress = big_to_lil(addr);
			// get number
			fread(&n, sizeof(unsigned short int), 1, my_obj_file);
			tmp_memory = search_tbl_by_address(memory, memoryAddress);
			//if search no result add new entry to tbl
			if(tmp_memory == NULL)
			{
				ret = add_entry_to_tbl(memory, memoryAddress, 0);
				if(ret == 0)
					tmp_memory = search_tbl_by_address(memory, memoryAddress);
				else // fail 
					return -1;
			}
			if(tmp_memory != NULL)
			{
				tmp_memory->label = (char*) malloc(sizeof(char)*(big_to_lil(n) + 1));
				for (i = 0; i < big_to_lil(n); i++) 
				{
					fread(&tmp_memory->label[i], sizeof(char), 1, my_obj_file);
				}
				tmp_memory->label[i]='\0';
			}
			
		}
	}
	return 0 ;
}
