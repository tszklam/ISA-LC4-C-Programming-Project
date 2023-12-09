/***************************************************************************
 * file name   : asm_parser.c                                              *
 * author      : tjf & you                                                 *
 * description : the functions are declared in asm_parser.h                *
 *               The intention of this library is to parse a .ASM file     *
 *			        										               * 
 *                                                                         *
 ***************************************************************************
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "asm_parser.h"

/* to do - implement all the functions in asm_parser.h */
int read_asm_file(char* filename, char program [ROWS][COLS] ) 
{
    FILE * fp = NULL;
    char buff[MAX_LEN] = {0};
    int cnt = 0;

    fp = fopen(filename,"r");
    if(fp == NULL)
    {
        printf("error2: read_asm_file() failed\n");
        return 2;
    }
    while(fgets(buff,MAX_LEN,fp))
    {
        if(strncmp(buff,";",1)==0)
            break;
        strtok(buff, "\n");
        memcpy(&program[cnt][0], buff, strlen(buff));
        cnt++;
    }
    fclose(fp);
    return 0;
}


int parse_instruction(char* instr, char* instr_bin_str)
{
    if(instr == NULL)
    {
        printf("error3: parse_instruction() failed.\n"); 
        return 3;
    }
    //parse OPCODE
    if(strncmp(instr,"ADD",3)==0)
    {
        parse_add(instr, instr_bin_str);
    }
    else if(strncmp(instr,"MUL",3)==0)
    {
        parse_mul(instr, instr_bin_str);
    }
    else if(strncmp(instr,"SUB",3)==0)
    {
        parse_sub(instr, instr_bin_str);
    }
    else if(strncmp(instr,"DIV",3)==0)
    {
        parse_div(instr, instr_bin_str);
    }
    else if(strncmp(instr,"AND",3)==0)
    {
        parse_and(instr, instr_bin_str);
    }
    else if(strncmp(instr,"OR",2)==0)
    {
        parse_or(instr, instr_bin_str);
    }
    else if(strncmp(instr,"XOR",3)==0)
    {
        parse_xor(instr, instr_bin_str);
    }
    else
    {
        printf("error3: parse_instruction() failed.\n"); 
        return 3;
    }

    return 0;
}



int parse_add(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error4: parse_add() failed.\n");
        return 4;
    }
    memcpy(instr_bin_str,"0001",4);
    memcpy(instr_bin_str+10,"000",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
             
            pos++;
        }
       
    }
    return 0;
}

int parse_mul(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error8: parse_mul() failed.\n");
        return 8;
    }
    memcpy(instr_bin_str,"0001",4);
    memcpy(instr_bin_str+10,"001",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
             
            pos++;
        }
       
    }
    return 0;
}

int parse_sub(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error9: parse_sub() failed.\n");
        return 9;
    }
    memcpy(instr_bin_str,"0001",4);
    memcpy(instr_bin_str+10,"010",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
            
            pos++;
        }
       
    }
    return 0;
}


int parse_div(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error10: parse_div() failed.\n");
        return 10;
    }
    memcpy(instr_bin_str,"0001",4);
    memcpy(instr_bin_str+10,"011",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
            
            pos++;
        }
       
    }
    return 0;
}

int parse_and(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error11: parse_and() failed.\n");
        return 11;
    }
    memcpy(instr_bin_str,"0101",4);
    memcpy(instr_bin_str+10,"000",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
            
            pos++;
        }
       
    }
    return 0;
}

int parse_or(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error12: parse_or() failed.\n");
        return 12;
    }
    memcpy(instr_bin_str,"0101",4);
    memcpy(instr_bin_str+10,"010",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
            
            pos++;
        }
       
    }
    return 0;
}

int parse_xor(char* instr, char* instr_bin_str )
{
    char *p = NULL;
    int pos = 0;
    if(instr == NULL)
    {
        printf(" error13: parse_xor() failed.\n");
        return 13;
    }
    memcpy(instr_bin_str,"0101",4);
    memcpy(instr_bin_str+10,"011",3);
    p = strtok(instr," ,");
    while(p)
    {
        p = strtok(NULL, " ,");
        if(p)
        {
            switch(pos)
            {
                case 0:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+4);
                    break;
                case 1:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+7);
                    
                    break;
                case 2:
                    parse_reg(*(p+strlen(p)-1), instr_bin_str+13);
                    break;
                default:
                    break;
            }
            
            pos++;
        }
       
    }
    return 0;
}

int parse_reg(char reg_num, char* instr_bin_str) 
{
    // printf("parse reg- %d\n", reg_num-'0');
    switch(reg_num-'0'){
        case 0:
            memcpy(instr_bin_str, "000", 3);
            break;
        case 1:
            memcpy(instr_bin_str, "001", 3);
            break;
        case 2:
            memcpy(instr_bin_str, "010", 3);
            break;
        case 3:
            memcpy(instr_bin_str, "011", 3);
            break;
        default:
            printf(" error5: parse_reg() failed.\n");
            return 5;   
    }
    return 0;
}

unsigned short int str_to_bin(char* instr_bin_str)
{
    char *ptr = NULL;
    long ret = 0;
    ret = strtol(instr_bin_str, &ptr, 2);
    if(ret == 0)
    {
        printf(" error6: str_to_bin() failed.\n");
        return 6;
    }
    return ret;
}


int write_obj_file(char* filename, unsigned short int program_bin[ROWS])
{
    int i=0;
    int rows = 0;
    char buff[MAX_LEN] = {0};
    FILE *fp = NULL;
    memcpy(buff,filename,strlen(filename)-3);
    sprintf(&buff[strlen(buff)],"obj");
    fp = fopen(buff, "wb");
    if(fp == NULL)
    {
        printf(" error7: write_obj_file() failed.\n");
        return 7;
    }
    memset(buff,0,strlen(buff));
    sprintf(buff,"%04X",0xCADE);
    fputs(buff,fp);
    memset(buff,0,strlen(buff));
    sprintf(buff,"%04X",0x0000);
    fputs(buff,fp);
    memset(buff,0,strlen(buff));
    for(i=0;i<ROWS;i++)
    {
        if(program_bin[i]!=0)
            rows++;
    }
    // printf("rows = %d\n",rows);
    sprintf(buff,"%04X",rows);
    fputs(buff,fp);
    for(i=0;i<rows;i++)
    {
        memset(buff,0,strlen(buff));
        sprintf(buff,"%04X",program_bin[i]);
        fputs(buff,fp);
    }
    fclose(fp);
    return 0;
}