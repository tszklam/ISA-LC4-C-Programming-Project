/************************************************************************/
/* File Name : lc4.c 													*/
/* Purpose   : This file contains the main() for this project			*/
/*             main() will call the loader and disassembler functions	*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"
#include "lc4_hash.h"
#include "lc4_loader.h"
#include "lc4_disassembler.h"

/* program to mimic pennsim loader and disassemble object files */

//hash function
int hash_by_address(void *table, void *address)
{
	//User Program Memory (x0000-1FFF), User Data Memory (x2000-x7FFF), 
	//OS Program Memory (x8000-x9FFF), OS Data Memory (xA000-xFFFF)
	int bucket;
	lc4_memory_segmented *mem = table;
	unsigned short *addr = address;
	if(*addr >= 0x0000 && *addr <= 0x1FFF)
		bucket = 0;
	else if(*addr >= 0x2000 && *addr <= 0x7FFF)
		bucket = 1;
	else if(*addr >= 0x8000 && *addr <= 0x9FFF)
		bucket = 2;
	else //if(*addr >= 0xA000 && *addr <= 0xFFFF)
		bucket = 3;
	// printf("hash 0x%04x -------- %d\n", *addr, bucket);
	return bucket;
}

int main (int argc, char** argv) {

	/**
	 * main() holds the hashtable &
	 * only calls functions in other files
	 */
	char* filename = NULL ;
    FILE *fp = NULL;
    int i = 0;
    if(argc < 2)
    {
        printf("error1: usage: ./lc4 <output.txt> ...\n");
        return -1;
    }
    if(argc < 3)
    {
        printf("error2: usage: ./lc4 <output.txt> <object_file.obj> ...\n");
        return -1;
    }
	/* step 1: create a pointer to the hashtable: memory 	*/
	lc4_memory_segmented* memory = NULL ;
	/* step 2: call create_hash_table() and create 4 buckets 	*/
	memory = create_hash_table (4, hash_by_address); 
    /*repeat steps 3 and 4 for each .OBJ file in argv[] 	*/
    for(i=2;i<argc;i++)
    {
        filename = argv[i];
        /* step 3: extract filename from argv, pass it to open_file() determine filename, then open it		*/
        fp = open_file(filename);
        if(fp == NULL)
        	continue;
        /* step 4: call function: parse_file() in lc4_loader.c 	*/
        parse_file(fp, memory);
        fclose(fp);
    }
	/* step 6: call function: reverse_assemble() in lc4_disassembler.c */
	if(reverse_assemble(memory))
	{
		delete_table(memory);
		return -1;
	}
	/* step 7: call function: print out the hashtable to output file */
	fp = fopen(argv[1], "w");
	if(fp == NULL)
	{
		delete_table(memory);
		return -1;
	}
	print_table(memory, fp);
	fclose(fp);
	/* step 8: call function: delete_table() in lc4_hash.c */
	delete_table(memory);
	/* only return 0 if everything works properly */
	return 0 ;
}
