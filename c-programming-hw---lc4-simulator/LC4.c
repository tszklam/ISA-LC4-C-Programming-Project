/*
 * LC4.c: Defines simulator functions for executing instructions
 */

#include "LC4.h"
#include <stdio.h>
#define INSN_OP(I) ((I >> 12) & 0x000F)
#define INSN_11_9(I) (((I) >> 9) & 0x7)
#define INSN_8_6(I) (((I) >> 6) & 0x7)
#define INSN_5_3(I) (((I) >> 3) & 0x7)
#define INSN_2_0(I) ((I) & 0x7)
#define INSN_IMM(I) (((I) >> 5) & 0x1)
#define INSN_COM_TYPE(I) (((I) >> 7) & 0x3)
#define INSN_JUMP_TYPE(I) (((I) >> 11) & 0x1)
#define INSN_SHIFT_TYPE(I) (((I) >> 4) & 0x3)

char* ToBinaryString(unsigned short int ins);
signed short int SextoI(unsigned short int ins, int length);

/*
 * Reset the machine state as Pennsim would do
 */
void Reset(MachineState* CPU)
{
    int i = 0; 

    CPU->PC = 0x8200 ; 
    CPU->PSR = 0x8002 ; 
    for (i = 0; i < 8; i++) {
        CPU -> R[i] = 0 ; 
    } 
    for (i = 0 ; i <= 65536; i++) {
        CPU->memory[i] = 0 ; 
    }
    ClearSignals(CPU);
}


/*
 * Clear all of the control signals (set to 0)
 */
void ClearSignals(MachineState* CPU)
{
    CPU->rsMux_CTL = 0;
    CPU->rtMux_CTL = 0;
    CPU->rdMux_CTL = 0;
    CPU->regFile_WE = 0;
    CPU->NZP_WE = 0;
    CPU->DATA_WE = 0;
}


/*
 * This function should write out the current state of the CPU to the file output.
 */
void WriteOut(MachineState* CPU, FILE* output)
{
    if(CPU->regFile_WE == 0)
    {
        CPU->rdMux_CTL = 0;
        CPU->regInputVal = 0;
    }
    if(CPU->NZP_WE == 0)
    {
        CPU->NZPVal = 0;
    }
    if(CPU->DATA_WE == 0 && INSN_OP(CPU->memory[CPU->PC]) != 6)// not load
    {
        CPU->dmemAddr = 0;
        CPU->dmemValue = 0;
    }
    // printf("%04X %s %01X %01X %04X %01X %01X %01X %04X %04X\n", CPU->PC, ToBinaryString(CPU->memory[CPU->PC]), CPU->regFile_WE, CPU->rdMux_CTL, CPU->regInputVal, CPU->NZP_WE, CPU->NZPVal, CPU->DATA_WE, CPU->dmemAddr, CPU->dmemValue);
    fprintf(output, "%04X %s %01X %01X %04X %01X %01X %01X %04X %04X\n", \
        CPU->PC, ToBinaryString(CPU->memory[CPU->PC]), CPU->regFile_WE, \
        CPU->rdMux_CTL, CPU->regInputVal, CPU->NZP_WE, CPU->NZPVal, CPU->DATA_WE,\
        CPU->dmemAddr, CPU->dmemValue);

}

/*
 * This function should execute one LC4 datapath cycle.
 */
int UpdateMachineState(MachineState* CPU, FILE* output)
{
    char *instruction_bits;
    unsigned short int instruction;
    unsigned short int immx, uimmx;
    instruction = CPU->memory[CPU->PC];
    instruction_bits = ToBinaryString(instruction);
    // printf("%s\n", instruction_bits);
    int opcode, sub_opcode;
    if((CPU->PC >= 0x2000 && CPU->PC <= 0x7FFF) || (CPU->PC >= 0xA000 && CPU->PC <= 0xFFFF)) 
    {
        printf("Error: Attempting to execute a data section address as code.\n");
        return 1;
    }
    if (CPU->DATA_WE == 1 && CPU->dmemAddr < 0x2000) { //opcode == 7
        printf("Error: Attempting to read or write a code section address as data.\n") ; 
        return 2 ; 
    }
    if(CPU->PSR == 0 && CPU->PC >= 0x8000) 
    {
        printf("Error: The processor is in user mode attempting to access an address or instruction in the OS section of memory\n");
        return 3;
    }  
    opcode = INSN_OP(instruction);
    CPU->rdMux_CTL = INSN_11_9(instruction);  
    CPU->rsMux_CTL = INSN_8_6(instruction); 
    sub_opcode =  INSN_5_3(instruction);       
    CPU->rtMux_CTL = INSN_2_0(instruction);   
    switch(opcode)
    {
        case 0://branch
            BranchOp(CPU, output);
            break;
        case 1://arith
            ArithmeticOp(CPU, output);
            break;
        case 2://compare
            ComparativeOp(CPU, output);
            break;
        case 4://jump subroutine
            JSROp(CPU, output);
            break;
        case 5://logic
            LogicalOp(CPU, output);
            break;
        case 6://load
            // printf("load\n");
            CPU->regFile_WE = 1;
            CPU->NZP_WE = 1;
            CPU->DATA_WE = 0;
            CPU->dmemAddr = (CPU->R[CPU->rsMux_CTL] + SextoI(instruction, 6)) & 0xFFFF;

            if ((CPU->PSR == 1 && CPU->dmemAddr >= 0x2000 && CPU->dmemAddr <= 0x7FFF) || 
                (CPU->PSR == 1 && CPU->dmemAddr >= 0xA000 && CPU->dmemAddr <= 0xFFFF) || 
                (CPU->PSR == 0 && CPU->dmemAddr >= 0x2000 && CPU->dmemAddr <= 0x7FFF)) 
            {
                CPU->dmemValue = CPU->memory[CPU->dmemAddr];
                CPU->regInputVal = CPU->dmemValue;
                CPU->R[CPU->rdMux_CTL] = CPU->dmemValue;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            } 
            else 
            { // error
                printf("ERROR: ILLEGAL ACCESS OPCODE == load\n");
                return -1;
            }     
            break;
        case 7://store
            // printf("store\n");
            CPU->regFile_WE = 0;
            CPU->NZP_WE = 0;
            CPU->DATA_WE = 1;
            CPU->dmemAddr = (CPU->R[CPU->rsMux_CTL] + SextoI(instruction, 6)) & 0xFFFF;
            if ((CPU->PSR == 1 && CPU->dmemAddr >= 0x2000 && CPU->dmemAddr  <= 0x7FFF) || 
                (CPU->PSR == 1 && CPU->dmemAddr >= 0xA000 && CPU->dmemAddr  <= 0xFFFF) || 
                (CPU->PSR == 0 && CPU->dmemAddr >= 0x2000 && CPU->dmemAddr <= 0x7FFF)) 
            {
                CPU->dmemValue = CPU->R[CPU->rdMux_CTL];
                CPU->memory[CPU->dmemAddr] = CPU->dmemValue;
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            } 
            else 
            { // error
                printf("ERROR: ILLEGAL ACCESS OPCODE == store\n");
                return -1;
            }       
        case 8://RTI
            // printf("RTI\n");
            CPU->PSR = 0;
            CPU->regFile_WE = 0;
            CPU->NZP_WE = 0;
            CPU->DATA_WE = 0;
            WriteOut(CPU, output);
            CPU->PC = CPU->R[7];
            break;
        case 9://const
            // printf("const\n");
            CPU->regInputVal = SextoI(instruction, 9);
            CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
            CPU->regFile_WE = 1;
            CPU->NZP_WE = 1;
            CPU->DATA_WE = 0;
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->PC++;
            break;
        case 10://shift and mod
            ShiftModOp(CPU, output);
            break;
        case 12://jump
            JumpOp(CPU, output);
            break;
        case 13://hiconst
            // printf("hiconst\n");
            CPU->regFile_WE = 1;
            CPU->NZP_WE = 1;
            CPU->DATA_WE = 0;
            CPU->regInputVal = (CPU->R[CPU->rdMux_CTL] & 0x00FF) | ((instruction & 0x00FF) << 8);        
            CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->PC++;
            break;
        case 15://trap
            // printf("trap\n");
            CPU->PSR = 1;
            CPU->regFile_WE = 1;
            CPU->NZP_WE = 1;
            CPU->DATA_WE = 0;
            CPU->regInputVal = CPU->PC+1;
            CPU->rdMux_CTL = 7;
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->R[7] = CPU->PC+1;
            CPU->PC = (0x8000 | (instruction & 0x00FF));
            break;
        default:
            printf("Opcode error, can not parse\n");
            return -1;
    }      

    return 0;
}



//////////////// PARSING HELPER FUNCTIONS ///////////////////////////



/*
 * Parses rest of branch operation and updates state of machine.
 */
void BranchOp(MachineState* CPU, FILE* output)
{
    // printf("BranchOp ");
    unsigned short int immx;
    immx= SextoI(CPU->memory[CPU->PC], 9);
    CPU->regInputVal = immx;
    CPU->regFile_WE = 0;
    CPU->NZP_WE = 0;
    CPU->DATA_WE = 0;
    switch(CPU->rdMux_CTL)
    {
        case 0://NOP
            // printf("nop\n");
            // WriteOut(CPU, output);
            CPU->PC++;
            break;
        case 1:// BRp (001)
             // printf("BRp\n");
            
            if(CPU->NZPVal == 1) 
            {
                WriteOut(CPU, output);
                CPU->PC = CPU->PC + 1 + immx;
            } 
            else 
            {
                WriteOut(CPU, output);
                CPU->PC++;
            }
            break;
        case 2:// BRz (010)
            // printf("BRz\n");
            if(CPU->NZPVal == 2) 
            {
                WriteOut(CPU, output);
                CPU->PC = CPU->PC + 1 + immx;
            } 
            else 
            {
                WriteOut(CPU, output);
                CPU->PC++;
            }
            break;
        case 3:// BRzp (010 or 001)
            // printf("BRzp\n");
            
            if(CPU->NZPVal == 1 || CPU->NZPVal == 2) 
            {
                WriteOut(CPU, output);
                CPU->PC = CPU->PC + 1 + immx;
            } 
            else 
            {
                WriteOut(CPU, output);
                CPU->PC++;
            }
            break;
        case 4:// BRn (100)
            // printf("BRn\n");
            
            if(CPU->NZPVal == 4) 
            {
                WriteOut(CPU, output);
                CPU->PC = CPU->PC + 1 + immx;
            } 
            else 
            {
                WriteOut(CPU, output);
                CPU->PC++;
            }
            break;
        case 5:// BRnp (100 or 001)
            // printf("BRnp\n");
            
            if(CPU->NZPVal == 4 || CPU->NZPVal == 1) 
            {
                WriteOut(CPU, output);
                CPU->PC = CPU->PC + 1 + immx;
            } 
            else 
            {
                WriteOut(CPU, output);
                CPU->PC++;
            }
            break;
        case 6:// BRnz (100 or 010)
            // printf("BRnz\n");
            if(CPU->NZPVal == 4 || CPU->NZPVal == 2) 
            {
                WriteOut(CPU, output);
                CPU->PC = CPU->PC + 1 + immx;
            } 
            else 
            {
                WriteOut(CPU, output);
                CPU->PC++;
            }
            break;
        case 7:// BRnzp
            // printf("BRnzp\n");
            WriteOut(CPU, output);

            CPU->PC = CPU->PC + 1 + immx;
            break;
        default:
            printf("INVALID INSTRUCTION\n");
            return;
    }
}

/*
 * Parses rest of arithmetic operation and prints out.
 */
void ArithmeticOp(MachineState* CPU, FILE* output)
{
    // printf("ArithmeticOp ");
    CPU->regFile_WE = 1;
    CPU->NZP_WE = 1;
    CPU->DATA_WE = 0;
    if(INSN_IMM(CPU->memory[CPU->PC]))
    {
        // printf("Add immediate\n");
        CPU->regInputVal = SextoI(CPU->memory[CPU->PC], 5);
        CPU->regInputVal += CPU->R[CPU->rsMux_CTL];
        CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
        SetNZP(CPU, CPU->regInputVal);
        WriteOut(CPU, output);
        CPU->PC++;
    }
    else
    {
        switch(INSN_5_3(CPU->memory[CPU->PC]))
        {
            case 0://add
                // printf("Add\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] + CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            case 1://mul
                // printf("Mul\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] * CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            case 2://sub
                // printf("Sub\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] - CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            case 3://div
                // printf("Div\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] / CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            default:
                printf("INVALID INSTRUCTION\n");
                return;
        }
    }
}

/*
 * Parses rest of comparative operation and prints out.
 */
void ComparativeOp(MachineState* CPU, FILE* output)
{
    // printf("ComparativeOp\n");
    CPU->regFile_WE = 0;
    CPU->NZP_WE = 1;
    CPU->DATA_WE = 0;
    switch(INSN_COM_TYPE(CPU->memory[CPU->PC]))
    {
        case 0:
        case 1:
            CPU->regInputVal = CPU->R[CPU->rdMux_CTL] - CPU->R[CPU->rtMux_CTL];
            break;
        case 2:
            CPU->regInputVal = CPU->R[CPU->rdMux_CTL] - SextoI(CPU->memory[CPU->PC], 7);
            break;
        case 3:
            CPU->regInputVal = CPU->R[CPU->rdMux_CTL] - (CPU->memory[CPU->PC] & 0x007F);
            break;
        default:
            printf("INVALID INSTRUCTION\n");
            return;
    }
    SetNZP(CPU, CPU->regInputVal);
    WriteOut(CPU, output);
    CPU->PC++;
}

/*
 * Parses rest of logical operation and prints out.
 */
void LogicalOp(MachineState* CPU, FILE* output)
{
    // printf("LogicalOp\n");
    CPU->regFile_WE = 1;
    CPU->NZP_WE = 1;
    CPU->DATA_WE = 0;
    if(INSN_IMM(CPU->memory[CPU->PC]))
    {
        // printf("And immediate\n");
        CPU->regInputVal = CPU->R[CPU->rsMux_CTL] & SextoI(CPU->memory[CPU->PC], 5);
        CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
        SetNZP(CPU, CPU->regInputVal);
        WriteOut(CPU, output);
        CPU->PC++;
    }
    else
    {
        switch(INSN_5_3(CPU->memory[CPU->PC]))
        {
            case 0://and
                // printf("And\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] & CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            case 1://not
                // printf("Not\n");
                CPU->regInputVal = ~CPU->R[CPU->rsMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            case 2://or
                // printf("Or\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] | CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            case 3://xor
                // printf("Xor\n");
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] ^ CPU->R[CPU->rtMux_CTL];
                CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
                SetNZP(CPU, CPU->regInputVal);
                WriteOut(CPU, output);
                CPU->PC++;
                break;
            default:
                printf("INVALID INSTRUCTION\n");
                return;
        }
    }
}

/*
 * Parses rest of jump operation and prints out.
 */
void JumpOp(MachineState* CPU, FILE* output)
{
    // printf("JumpOp\n");
    CPU->regFile_WE = 0;
    CPU->NZP_WE = 0;
    CPU->DATA_WE = 0;
    switch(INSN_JUMP_TYPE(CPU->memory[CPU->PC]))
    {
        case 0://jmpr
            WriteOut(CPU, output);
            CPU->PC = CPU->R[CPU->rsMux_CTL];
            break;
        case 1://jmp
            WriteOut(CPU, output);
            CPU->PC = CPU->PC + 1 + SextoI(CPU->memory[CPU->PC], 11);
            break;
        default:
            printf("INVALID INSTRUCTION\n");
            return;
    }
}

/*
 * Parses rest of JSR operation and prints out.
 */
void JSROp(MachineState* CPU, FILE* output)
{
    // printf("JSROp\n");
    CPU->regFile_WE = 1;
    CPU->NZP_WE = 1;
    CPU->DATA_WE = 0;
    switch(INSN_JUMP_TYPE(CPU->memory[CPU->PC]))
    {
        case 0://jsrr
            CPU->regInputVal = CPU->PC + 1;
            SetNZP(CPU, CPU->regInputVal);
            CPU->rdMux_CTL = 7;
            WriteOut(CPU, output);
            CPU->PC = CPU->R[CPU->rsMux_CTL];
            CPU->R[7] = CPU->regInputVal;
            break;
        case 1://jsr
            CPU->regInputVal = CPU->PC + 1;
            SetNZP(CPU, CPU->regInputVal);
            CPU->rdMux_CTL = 7;
            WriteOut(CPU, output);
            CPU->PC = (CPU->PC & 0x8000) | (SextoI(CPU->memory[CPU->PC], 11) << 4);
            CPU->R[7] = CPU->regInputVal;
            break;
        default:
            printf("INVALID INSTRUCTION\n");
            return;
    }
}

/*
 * Parses rest of shift/mod operations and prints out.
 */
void ShiftModOp(MachineState* CPU, FILE* output)
{
    // printf("ShiftModOp\n");
    CPU->regFile_WE = 1;
    CPU->NZP_WE = 1;
    CPU->DATA_WE = 0;
    switch(INSN_SHIFT_TYPE(CPU->memory[CPU->PC]))
    {
        case 0://sll
            CPU->regInputVal = CPU->R[CPU->rsMux_CTL] << (CPU->memory[CPU->PC] & 0x000F);
            CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->PC++;
            break;
        case 1://sra
            if(CPU->R[CPU->rsMux_CTL] >= 0) // positive
            { 
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] << (CPU->memory[CPU->PC] & 0x000F);
            } 
            else // negative, sign extend
            { 
                CPU->regInputVal = CPU->R[CPU->rsMux_CTL] >> (CPU->memory[CPU->PC] & 0x000F);
                CPU->regInputVal = SextoI(CPU->regInputVal, 16 - (CPU->memory[CPU->PC] & 0x000F));
            }           
            CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->PC++;
            break;
        case 2://srl
            CPU->regInputVal = CPU->R[CPU->rsMux_CTL] >> (CPU->memory[CPU->PC] & 0x000F);
            CPU->R[CPU->rdMux_CTL] = CPU->regInputVal;
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->PC++;
            break;
        case 3://mod
            CPU->regInputVal = CPU->R[CPU->rsMux_CTL] % CPU->R[CPU->rtMux_CTL];
            SetNZP(CPU, CPU->regInputVal);
            WriteOut(CPU, output);
            CPU->PC++;
            break;
        default:
            printf("INVALID INSTRUCTION\n");
            return;
    }
}

/*
 * Set the NZP bits in the PSR.
 */
void SetNZP(MachineState* CPU, short result)
{
    // printf("SetNZP\n");
    if(result == 0)  // 010 (0)
    {
        CPU->NZPVal =  2;
    } 
    else if(((result >> 15) & 0x0001) == 1) // 100 (-)
    { 
        CPU->NZPVal = 4;
    } 
    else // 001 (+)
    { 
        CPU->NZPVal = 1;
    }
}

/*
 * instructions to character string 
 */
char* ToBinaryString(unsigned short int ins) 
{
    char *ptr = malloc(sizeof(char) * 17);
    unsigned int i;
    for (i = 1 << 15; i > 0; i = i / 2) 
    {
        if (ins & i) 
        {    
            *ptr = '1';
        } 
        else 
        {    
            *ptr = '0';
        }
        ptr++;
    }
    *ptr = '\0'; // null termination
    ptr = ptr - 16; // reset pointer
    return ptr;
}

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
