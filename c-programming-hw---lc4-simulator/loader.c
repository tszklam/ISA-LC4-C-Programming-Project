/*
 * loader.c : Defines loader functions for opening and loading object files
 */

#include "loader.h"

// memory array location
unsigned short memoryAddress;

//Endianness
unsigned short int big_to_lil(unsigned short int x) {
  short int upper, lower, final;
  upper = (x & 0xFF00) >> 8; 
  lower = (x & 0x00FF) << 8; 
  final = upper | lower;
  return final;
}

/*
 * Read an object file and modify the machine state as described in the writeup
 */
int ReadObjectFile(char* filename, MachineState* CPU)
{
  FILE *fp = NULL;
  unsigned short int buff, addr,n,words;
  unsigned int file_length_bytes;
  int i = 0, j = 0;
  fp = fopen(filename,"rb");
  if(fp == NULL)
  {
    printf("error2:  file %s not exist\n", filename);
    return -1;
  }
  while(fread(&buff, sizeof(unsigned short int), 1, fp) == 1) 
  { 
    // look for headers
    if(buff == 0xDECA) //code setion
    {
      //get init address
      fread(&addr, sizeof(unsigned short int), 1, fp);
      memoryAddress = big_to_lil(addr);
      //get number
      fread(&n, sizeof(unsigned short int), 1, fp);
      for (i = 0; i < big_to_lil(n); i++) {
        fread(&words, sizeof(unsigned short int), 1, fp);
        CPU->memory[memoryAddress+i] = big_to_lil(words);
      }
    } 
    else if(buff == 0xDADA) // data setion
    { 
      // get init address
      fread(&addr, sizeof(unsigned short int), 1, fp);
      memoryAddress = big_to_lil(addr);
      // get number
      fread(&n, sizeof(unsigned short int), 1, fp);
      for (i = 0; i < big_to_lil(n); i++) 
      {
        fread(&words, sizeof(unsigned short int), 1, fp);
        CPU->memory[memoryAddress++] = big_to_lil(words);
      }
    } 
  }
  fclose(fp);
  return 0;
}
