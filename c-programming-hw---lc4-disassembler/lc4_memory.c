/************************************************************************/
/* File Name : lc4_memory.c		 										*/
/* Purpose   : This file implements the linked_list helper functions	*/
/* 			   to manage the LC4 memory									*/
/*             															*/
/* Author(s) : tjf and you												*/
/************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "lc4_memory.h"


/*
 * adds a new node to linked list pointed to by head
 */
int add_to_list (row_of_memory** head,
		 		 unsigned short int address,
		 		 unsigned short int contents)
{

	/* allocate memory for a single node */
	row_of_memory *new_memory = (row_of_memory*) malloc(sizeof(row_of_memory));
	if(new_memory == NULL)
		return -1;
	/* populate fields in newly allocated node with arguments: address/contents */
	new_memory->address = address;
	new_memory->contents = contents;
    /* make certain to set other fields in structure to NULL */
	new_memory->label = NULL;
	new_memory->assembly = NULL;
	new_memory->next = NULL;
	/* if head==NULL, node created is the new head of the list! */
	if(*head==NULL)
	{
		*head = new_memory;
	}
	else
	{
		/* otherwise, insert the node into the linked list keeping it in order of ascending addresses */
		if((*head)->address >= address)
		{
			new_memory->next = *head;
			*head = new_memory;
		}
		else
		{
			row_of_memory *prev_memory, *tmp_memory;
			prev_memory = *head;
			tmp_memory = prev_memory;
			while((prev_memory->next != NULL) && (address > prev_memory->next->address)) 
			{
				prev_memory = prev_memory->next;
			}
			new_memory->next = prev_memory->next;
			prev_memory->next = new_memory;
			// *head = tmp_memory->next;
		}
	}
	/* return 0 for success, -1 if malloc fails */
	return 0 ;
}


/*
 * search linked list by address field, returns node if found
 */
row_of_memory* search_address (row_of_memory* head,
			        		   unsigned short int address )
{
	// printf("search_address\n");
	row_of_memory *tmp_memory;
	
	/*list is empty*/
	if(head == NULL)
	{
		// printf("list is empty\n");
		return NULL;
	}
	tmp_memory = head;
	/* traverse linked list, searching each node for "address"  */
	while(tmp_memory != NULL && tmp_memory->address != address)
	{
		tmp_memory = tmp_memory->next;
	}
	/*"address" isn't found*/
	if(tmp_memory == NULL)
	{
		//printf("address isn't found\n");
		return NULL;
	}
	// printf("find 0x%04x: contents = 0x%04x\n", address, tmp_memory->contents);
	/* return pointer to node in the list if item is found */
	return tmp_memory ;
}

/*
 * search linked list by opcode field, returns node if found
 */
row_of_memory* search_opcode (row_of_memory* head,
				      		  unsigned short int opcode  )
{
	// printf("search_opcode: 0x%04x\n", opcode);
	row_of_memory *tmp_memory;
	
	/*list is empty*/
	if(head == NULL)
	{
		// printf("list is empty\n");
		return NULL;
	}
	tmp_memory = head;
	/* traverse linked list until node is found with matching opcode
	   AND "assembly" field of node is empty */
	while(tmp_memory != NULL && (((tmp_memory->contents)>>12) != opcode || tmp_memory->assembly != NULL))
	{
		tmp_memory = tmp_memory->next;
	}
	/* opcode isn't found*/
	if(tmp_memory == NULL)
	{
		// printf("opcode isn't found\n");
		return NULL;
	}
	// printf("find 0x%04x: contents = 0x%04x\n", opcode, tmp_memory->contents);
	/* return pointer to node in the list if item is found */
	return tmp_memory ;
}

/*
 * delete the node from the linked list with matching address
 */
int delete_from_list (row_of_memory** head,
			          unsigned short int address ) 
{
	/* if head isn't NULL, traverse linked list until node is found with matching address */
	if(*head!=NULL)
	{
		row_of_memory *prev_memory;
		row_of_memory *tmp_memory;
		prev_memory = *head;
		//first node 
		if((*head)->address == address)
		{
			*head = (*head)->next;
			free(prev_memory->label);
			free(prev_memory->assembly);
			free(prev_memory);
			return 0;
		}
		while((prev_memory->next != NULL) && (prev_memory->next->address != address)) 
		{
			prev_memory = prev_memory->next;
		}
		 // delete the matching node, re-link the list 
		if(prev_memory->next == NULL)
		{
			printf("Node wasn't found\n");
			return -1;
		}
		tmp_memory = prev_memory->next;
		prev_memory->next = tmp_memory->next;
		free(tmp_memory->label);
		free(tmp_memory->assembly);
		free(tmp_memory);
		/* make certain to update the head pointer - if original was deleted */
		
		/* return 0 if successfully deleted the node from list, -1 if node wasn't found */
		return 0;
	}
	else
	{
		printf("empty no node to delete\n");
		return -1;
	}
}

void print_list (row_of_memory* head, 
				 FILE* output_file )
{
	//int count = 0;
	
	/* make sure head isn't NULL */
	if(head==NULL)
	{
		// printf("list is empty\n");
		return;
	}
	/* print out a header to output_file */

	/* traverse linked list, print contents of each node to output_file */
	row_of_memory *tmp_memory = head;
	while(tmp_memory != NULL) 
	{
		// printf("-------------------------------\n");
		// printf("Node #%d: address = 0x%04x, contents = 0x%04x, label = %s, assembly = %s\n", 
		// 	count, tmp_memory->address, tmp_memory->contents,tmp_memory->label, tmp_memory->assembly);
		fprintf(output_file,"%-20s %04X %-15s %04X %-15s %-20s\n",
			(tmp_memory->label==NULL)?"":tmp_memory->label, tmp_memory->address, 
			"", tmp_memory->contents, "", (tmp_memory->assembly==NULL)?"":tmp_memory->assembly);
		tmp_memory = tmp_memory->next;
		//count++;
	}
	return ;
}

/*
 * delete entire linked list
 */
void delete_list (row_of_memory** head )
{
	if(*head == NULL){
		return; 
	}
	/* delete entire list node by node */
	while((*head)->next!=NULL){
		row_of_memory *tmp_memory = (*head)->next;
		(*head)->next = tmp_memory->next;
		free(tmp_memory->label);
		free(tmp_memory->assembly);
		free(tmp_memory);
	}
	/* set head = NULL upon deletion */
	free((*head)->label);
	free((*head)->assembly);
	free((*head));
	(*head) = NULL;
	return ;
}