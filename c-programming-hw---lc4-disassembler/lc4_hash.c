/************************************************************************/
/* File Name : lc4_hash.c		 										*/
/* Purpose   : This file contains the definitions for the hash table  	*/
/*																		*/
/* Author(s) : tjf 														*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_hash.h"

/*
 * creates a new hash table with num_of_buckets requested
 */
lc4_memory_segmented* create_hash_table (int num_of_buckets, 
             					         int (*hash_function)(void* table, void *key) ) 
{
	// allocate a single hash table struct
	lc4_memory_segmented *my_table =(lc4_memory_segmented *) malloc(sizeof(lc4_memory_segmented));
	if(my_table == NULL)
		return NULL;
	// allocate memory for the buckets (head pointers)
	my_table->buckets = malloc(sizeof(row_of_memory*)*num_of_buckets);
	if(my_table->buckets ==NULL)
	{
		free(my_table);
		return NULL;
	}
	// Initialization buckets
	for(int i = 0; i<num_of_buckets; i++)
	{
		my_table->buckets[i] = NULL;
	}
	my_table->num_of_buckets = num_of_buckets ;
	// assign function pointer to call back hashing function
	my_table->hash_function = hash_function;
	// return table pointer to caller
	return my_table; 
}


/*
 * adds a new entry to the table
 */
int add_entry_to_tbl (lc4_memory_segmented* table, 
					  unsigned short int address,
			    	  unsigned short int contents) 
{
	
	int ret = 0;
	int bucket;
	if(table == NULL)
		return -1;
	// apply hashing function to determine proper bucket #
	bucket = table->hash_function(table, &address) ;
	// add to bucket's linked list using linked list add_to_list() helper function
	ret = add_to_list (&table->buckets[bucket], address, contents);
	if (ret != 0) 
		return -2;
	return 0 ;
}

/*
 * search for an address in the hash table
 */
row_of_memory* search_tbl_by_address 	(lc4_memory_segmented* table,
			                   			 unsigned short int address ) 
{
	int bucket;
	row_of_memory *tmp_memory;
	// apply hashing function to determine bucket # item must be located in
	// printf("search_address = 0x%04x\n",address);
	bucket = table->hash_function(table, &address);
	// invoked linked_lists helper function, search_by_address() to return return proper node
	tmp_memory = search_address(table->buckets[bucket],address);
	return tmp_memory;
}

/*
 * prints the linked list in a particular bucket
 */

void print_bucket (lc4_memory_segmented* table, 
				   int bucket_number,
				   FILE* output_file ) 
{
	// call the linked list helper function to print linked list
	print_list(table->buckets[bucket_number], output_file);
	return ;
}

/*
 * print the entire table (all buckets)
 */
void print_table (lc4_memory_segmented* table, 
				  FILE* output_file ) 
{
	// call the linked list helper function to print linked list to output file for each bucket
	int i;
	if(table == NULL)
		return;
	fprintf(output_file, "%-20s %-20s %-20s %-20s\n","<label>","<address>","<contents>","<assembly>");
	for(i=0;i<table->num_of_buckets;i++)
	{
		print_list(table->buckets[i], output_file);
	}
	return ;
}

/*
 * delete the entire table and underlying linked lists
 */

void delete_table (lc4_memory_segmented* table ) 
{
	// call linked list delete_list() on each bucket in hash table
	int i;
	for(i=0;i<table->num_of_buckets;i++)
	{
		delete_list(&table->buckets[i]);
		// free(table->buckets[i]);
		// table->buckets[i] = NULL;
	}
	// then delete the table itself
	free(table->buckets);
	free(table);
	table = NULL;
	return ;
}
