/*

This program emulates ARM instructions using C. Given a file with ARM (technically LEGv8) instructions,
It will perform any operation included in the defined set and output the results to the console. The program
exits when it either branches to XZR or if the PC attempts to execute an unrecognized or empty instruction.

In this upgrade to the program, I have added stack functionality, with a STACK POINTER (SP) Register and
checks on whether or not pushing/popping will exceed the allotted memory space for the program, or push 
lower than the minimum bounds allotted to the stack itself.

I have also added the Branch and Link (BL) operation for executing function calls.

Added functionality to pause execution each instruction, and print the stack contents, if any, based on the
current stack pointer.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// MACRO CONSTANTS

#define STARTMEM 200
#define MEMSIZE 4096
#define STACKSIZE 1024
#define LINESIZE 255
#define WORD 4
#define DOUBLEWORD 8
#define STACKUPPERBOUND MEMSIZE -1
#define REGISTERSPACE 32
#define SP 28
#define PC 29
#define LR 30
#define XZR 31

// OPERATION LABELS

#define ADD 1112
#define ADDI 580
#define SUB 1624
#define SUBI 836
#define LDUR 1986
#define STUR 1984
#define LSL 1691
#define CBZ 180
#define CBNZ 181
#define BR 1712
#define BL 31
#define B 5

// PROTOTYPES

void parseBuffer(char *string);
int getOperand(char *string);
int getInstruct(char *string);
int executeInstruction(int);
void ungetOperand(int, char*);
void printMemory(char, int, int);
void printStack();
void waitForAdvance();

// GLOBAL VARIABLES

int memory[MEMSIZE][WORD];
int registers[REGISTERSPACE];  
const int maxStack = STACKUPPERBOUND - STACKSIZE;
const char OVERFLOW_MSG[] = "WARNING: STACK OVER FLOW. Your program will now crash. Thank you.\n";

// FUNCTIONS 

// Accepts a String buffer and index to write to in the Text Segment, writes the given
// instruction into the Text Segment as integer psuedo-machine code.

void parseBuffer(char *buffer){

    char inst[5][10];
    int machineCode, i = 0;
    int segments = 4; // decrements based on instruction type
    
    // split the buffer into substrings delimited by spaces
    sscanf(buffer,"%s %s %s %s %s", inst[0], inst[1], inst[2], inst[3], inst[4]);

    int index = atoi(inst[i]);

    i++;
    for(int wipe = 0; wipe < 4; wipe++) {
        memory[index][wipe] = 0;            // clears the previous contents of the instruction
    }                                      // at this memory location 

    // convert substring to integers to store in memory
    while(i < 5 && segments) {

        if(i == 1) {
            machineCode = getInstruct(inst[i]);
            if(machineCode == CBZ || machineCode == CBNZ) {
                segments--;  // only accepts 2 operands
            } else if (machineCode == B || machineCode == BR || machineCode == BL) {
                segments-=2; // only accepts 1 operand
            }

        } else { 
            machineCode = getOperand(inst[i]);    
        }

        // write to memory
        memory[index][i-1] = machineCode;
        i++;
        segments--;
    }
}

// Converts the given instruction String into an integer to be held in the memory array

int getInstruct(char *instruction) {

        if((strcmp(instruction, "ADD") == 0)) {
            return ADD;
        } else if(strcmp(instruction, "ADDI") == 0) {
            return ADDI;
        } else if(strcmp(instruction, "SUB") == 0) {
            return SUB;
        } else if(strcmp(instruction, "SUBI") == 0) {
            return SUBI;
        } else if(strcmp(instruction, "LDUR") == 0) {
            return LDUR;
        } else if(strcmp(instruction, "STUR") == 0) {
            return STUR;
        } else if(strcmp(instruction, "LSL") == 0) {
            return LSL;
        } else if(strcmp(instruction, "CBZ") == 0) {
            return CBZ;
        } else if(strcmp(instruction, "CBNZ") == 0) {
            return CBNZ;
        } else if(strcmp(instruction, "BR") == 0) {
            return BR;
        } else if(strcmp(instruction, "B") == 0) {
            return B;
        } else if(strcmp(instruction, "BL") == 0) {
            return BL; 
        } else {
            return atoi(instruction);
        }
}

// Executes the correct operation based on the given instruction.

int executeInstruction(int instr) {
    int op1 = memory[instr][1];
    int op2 = memory[instr][2];      // variables for readability
    int op3 = memory[instr][3];

    switch(memory[instr][0]) {
        case ADD : 
            registers[op1] = registers[op2] + registers[op3];
            break;
        case ADDI :      
            registers[op1] = registers[op2] + op3;
            if(op2 == SP) {
                if(registers[op1] > (STACKUPPERBOUND)) {
                    printf("%s\n", OVERFLOW_MSG);             // check for overflow beyond upper bounds of memory when popping
                    return -1;                                // from the stack and increasing the SP
                }
            } 
            break;
        case SUB : 
            registers[op1] = registers[op2] - registers[op3];
            break;
        case SUBI : 
            registers[op1] = registers[op2] - op3;
            if(op2 == SP) {
                if(registers[op1] < maxStack) {
                    printf("%s\n", OVERFLOW_MSG);              // check for overflow beyond lower bounds when pushing to stack
                    return -1;
                }
            } 
            break;
        case LDUR : 
            registers[op1] = memory[registers[op2] + op3][0];
            break;
        case STUR : 
            memory[registers[op2] + op3][0] = registers[op1];
            break;
        case LSL :
            registers[op1] = registers[op2] << op3;
            break;
        case CBZ : 
            registers[PC] = (registers[op1] - registers[XZR] == 0) ? op2 : (registers[PC] + WORD);
            return 1;
        case CBNZ : 
            registers[PC] = (registers[op1] - registers[XZR] != 0) ? op2 : (registers[PC] + WORD);
            return 1;
        case BR : 
            if(op1 == XZR) {
                return 0;
            } else {
                registers[PC] = registers[op1];
                return 1;
            }
        case BL : 
            registers[LR] = registers[PC] + WORD; 
            registers[PC] = op1;
            return 1;
        case B : 
            registers[PC] = op1;
            return 1;
        default:
            return 0;
    }
    registers[PC] = registers[PC] + WORD; // incremements PC for non branch operations
    return 1;
}

// Formats output based on given instruction

void outputResult(int inst) {

    int opcode = memory[inst][0], op1 = memory[inst][1], op2 = memory[inst][2], op3 = memory[inst][3];
    char opStr1[32], opStr2[32], opStr3[32];
    ungetOperand(op1, opStr1); 
    ungetOperand(op2, opStr2);                          // Variables for readability
    ungetOperand(op3, opStr3);                          // "op" for operand

    if(memory[inst][0] == ADD) {
        printf("PC = %i, Instruction: ADD ", inst); 
        printf("%s, %s, %s\n", opStr1, opStr2, opStr3);
        printf("Registers: %s: %i | %s: %i | %s: %i\n", opStr1, registers[op1], opStr2, registers[op2], opStr3, registers[op3]);
    } else if(memory[inst][0] == ADDI) {
        printf("PC = %i, Instruction: ADDI ", inst); 
        printf("%s, %s, #%i\n", opStr1, opStr2, op3);
        printf("Registers: %s: %i | %s: %i | immediate: #%i\n", opStr1, registers[op1], opStr2, registers[op2], op3);
    } else if(memory[inst][0] == SUB) {
        printf("PC = %i, Instruction: SUB ", inst); 
        printf("%s, %s, %s\n", opStr1, opStr2, opStr3);
        printf("Registers: %s: %i | %s: %i | %s: %i\n", opStr1, registers[op1], opStr2, registers[op2], opStr3, registers[op3]);
    } else if(memory[inst][0] == SUBI) {
        printf("PC = %i, Instruction: SUBI ", inst); 
        printf("%s, %s, #%i\n", opStr1, opStr2, op3);
        printf("Registers: %s: %i | %s: %i | immediate: #%i\n", opStr1, registers[op1], opStr2, registers[op2], op3);
    } else if(memory[inst][0] == LDUR) {
        printf("PC = %i, Instruction: LDUR ", inst); 
        printf("%s, [%s, #%i]\n", opStr1, opStr2, op3);
        if(op2 == SP) {
            int differenceForPrintingStack = STACKUPPERBOUND - STACKSIZE;
            printf("Popped %i from stack memory address %x into register %s\n", registers[op1], 
            (registers[op2] + op3) - differenceForPrintingStack, opStr1);
        } else {
            printf("Register %s now contains %i from memory address %x\n", opStr1, registers[op1], registers[op2]);
        }
    } else if(memory[inst][0] == STUR) {
        printf("PC = %i, Instruction: STUR ", inst); 
        printf("%s, [%s, #%i]\n", opStr1, opStr2, op3);
        if(op2 == SP) {
            int differenceForPrintingStack = STACKUPPERBOUND - STACKSIZE;
            printf("Pushed %i to the stack at memory address %x\n", registers[op1], 
            (registers[op2] + op3) - differenceForPrintingStack);
        } else {
            printf("Memory address %x now contains %i\n", registers[op2] + op3, registers[op1]);
        }
    } else if(memory[inst][0] == LSL) {
        printf("PC = %i, Instruction: LSL ", inst); 
        printf("%s, %s, #%i\n", opStr1, opStr2, op3);
        printf("Registers: %s: %i | %s: %i | %s: %i\n", opStr1, registers[op1], opStr2, registers[op2], opStr3, registers[op3]);
    } else if(memory[inst][0] == CBZ) {
        printf("PC = %i, Instruction: CBZ X%i, %i\n", inst, op1, op2); 
        printf("Conditional branch on zero for register %s, containing %i\n", 
            opStr1, registers[op1]);
        printf(">>\tPC is now %i \n", registers[PC]);
    } else if(memory[inst][0]== CBNZ) {
        printf("PC = %i, Instruction: CBNZ X%i, %i\n", inst, op1, op2); 
        printf("Conditional branch on not zero for register %s, containing %i\n", 
            opStr1, registers[op1]);
        printf(">> \tPC is now %i \n", registers[PC]);
    } else if(memory[inst][0] == BR) {
        printf("PC = %i, Instruction: BR %s\n", inst, opStr1); 
        printf("Branching to instruction %i from register %s\n", registers[op1],  opStr1);
        printf(">>\tPC is now %i \n", registers[PC]);
    } else if(memory[inst][0] == BL) {
        printf("PC = %i, Instruction: BL %i\n", inst, op1); 
        printf("Branching to function at %i. Storing Instruction at %i in LR.\n", op1, inst+WORD);
        printf(">>\tPC is now %i. LR is %i \n", registers[PC], registers[LR]);
    }else if(memory[inst][0] == B) {
        printf("PC = %i, Instruction: B %i ", inst, op1); 
        printf("Branching to instruction %i\n", op1);
        printf(">>\tPC is now %i \n", registers[PC]);
    }

    printStack();
    printf("\n");
}

// Extracts the register/immediate number as an integer from the given String

int getOperand(char *operandStr) {

    // shifts index for STUR/LDUR commands where operand 2 starts with "[".
    int bracket = (operandStr[0] != '[') ? 0 : 1;
    if(operandStr[1 + bracket] == 'Z') { // check if XZR
        return XZR;
    } else if(operandStr[0 + bracket] == 'S') {
        return SP;
    } else if(operandStr[0 + bracket] == 'L') {
        return LR;
    }

    int i = 0, j = 0;
    char temp[32];

    while(operandStr[i] != ',' && operandStr[i] != '\0' ) {  
        
        if(operandStr[i] != 'X' && operandStr[i] != '#' 
        && operandStr[i] != '[' && operandStr[i] != ']') { 
            
            temp[j] = operandStr[i];
            j++;
        }
        i++;
    }
    temp[j] = '\0';

    return atoi(temp);
}

// Converts the given register back into a String and stores it in the String passed to it

void ungetOperand(int reg, char * registerStr) {
    
    if(reg == XZR) {
        strcpy(registerStr, "XZR");
    } else if (reg == SP) {
        strcpy(registerStr, "SP");
    } else if (reg == LR) {
        strcpy(registerStr, "LR");
    } else {
        sprintf(registerStr, "X%i", reg);
   }
}

// Accepts an enter press to advance the program.

void waitForAdvance() {
    char enter;
    while(enter != 0x0A) {
        printf("> ");
        enter = getchar();
    }
}

// prints contents of the given memory location, from start to end, to the console
//      'm' : memory
//      'r' : registers
// used for debugging


void printMemory(char which, int start, int end) {

    if(which == 'm') {
        for(int i = start; i <= end; i++) {
            printf("Memory address %i : %i %i %i %i\n", i, memory[i][0], memory[i][1],
            memory[i][2], memory[i][3]);
        }
    
    } else if(which == 'r') {
        for(int i = start; i <= end; i++) {
            printf("Register X%i : %i\n", i, registers[i]);
        }

    } else {
        printf("Argument to printMemory() not recognized.\n");
    }
}

// Prints the stack contents of the stack if there are any, from the top of memory
// to the current position of the stack pointer, inclusive. Formats text to present
// the stack position as a decrementing number from the maxium size of the stack.

void printStack() {
    int differenceForPrintingStack = STACKUPPERBOUND - STACKSIZE; // used as offset so stack prints in descending
    int i = STACKUPPERBOUND;                                      // order from STACKSIZE: i.e., 1024, 1023, ...

    if(i >= registers[SP]) {
        printf("Stack:\t%x : %i\n", i -differenceForPrintingStack, memory[i][0]); // prints the first stack doubleword
    }                                                                             // with the "Stack:" lead in text
    i-=DOUBLEWORD;
    while(i >= registers[SP]) {
        printf("\t%x : %i\n", i - differenceForPrintingStack, memory[i][0]);
        i-=DOUBLEWORD;
    }
}

// Main method //

int main(int argc, char *argv[]) {

    char buffer[LINESIZE];
    
    FILE* program = fopen(argv[1], "r");

    if(program == NULL || argc < 2 || argc > 2) {
        printf("Please try again with a valid file.\n");
        return -1;
    }

    // Takes the input from the given file

    while(fgets(buffer, LINESIZE, program)) {
        parseBuffer(buffer);
    }

    fclose(program);
        
    printf("Press ENTER to execute next instruction\n\n");

    registers[PC] = STARTMEM;
    registers[SP]= STACKUPPERBOUND; 

    int exec = 1, startingPC;
    while(exec) { 
        startingPC = registers[PC];     // saves the original memory location for output
        exec = executeInstruction(registers[PC]);
        if(exec < 0) {
            return -1;
        }

        if(exec) {
            printf("\n");
            outputResult(startingPC);
            waitForAdvance();
        } else {
            printf("Program complete. Now exiting.\n");
            break;
        }
    }
    
    // printMemory('m', 0, MEMSIZE -1);
    // printMemory('r', 0, 31); 
    return 0;
}