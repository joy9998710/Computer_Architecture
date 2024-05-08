/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    /* initialize state */
    state.pc = 0;
    for(int i = 0; i < NUMREGS; i++){
        state.reg[i] = 0;
    }
    int instructions = 0;

    printState(&state);

    while(1){
        /* Get opcode from memory */
        int opcode = (state.mem[state.pc] >> 22) & 7;
        /* R type format add | nor (0 or 1)*/
        if(opcode == 0 || opcode == 1){
            int reg1 = (state.mem[state.pc] >> 19) & 7;
            int reg2 = (state.mem[state.pc] >> 16) & 7;
            int destReg = (state.mem[state.pc]) & 7;
            /* if instruction is add */
            if(opcode == 0){
                state.reg[destReg] = state.reg[reg1] + state.reg[reg2];
            }
            /* if instruction is nor */
            else if(opcode == 1){
                state.reg[destReg] = ~(state.reg[reg1] | state.reg[reg2]);
            }
        }
        /* I type format lw | sw | beq (2, 3, 4) */
        else if(opcode == 2 || opcode == 3 || opcode == 4){
            int reg1 = (state.mem[state.pc] >> 19) & 7;
            int reg2 = (state.mem[state.pc] >> 16) & 7;
            int offset = (state.mem[state.pc] & 0xFFFF);
            offset = convertNum(offset);
            /* if instruction is lw */
            if(opcode == 2){
                state.reg[reg2] = state.mem[state.reg[reg1] + offset];
            }
            /* if instruction is sw */
            else if(opcode == 3){
                state.mem[state.reg[reg1] + offset] = state.reg[reg2];
            }
            /* if instruction is beq */
            else if(opcode == 4){
                if(state.reg[reg1] == state.reg[reg2]){
                    instructions++;
                    state.pc = state.pc + 1 + offset;
                    printState(&state);
                    continue;
                }
                else{
                    instructions++;
                    state.pc++;
                    printState(&state);
                    continue;
                }
            }
        }
        /* J type format jalr (5) */
        else if(opcode == 5){
            int reg1 = (state.mem[state.pc] >> 19) & 7;
            int reg2 = (state.mem[state.pc] >> 16) & 7;
            state.reg[reg2] = state.pc + 1;
            state.pc = state.reg[reg1];
            instructions++;
            printState(&state);
            continue;
        }
        /* O type format halt | noop */
        else if(opcode == 6){
            state.pc++;
            instructions++;
            break;
        }
        else if(opcode == 7){
            state.pc++;
            instructions++;
            printState(&state);
            continue;
        }
        instructions++;
        state.pc++;
        printState(&state);
    }
    printf("machine halted\n");
    printf("total of %d instructions executed\n", instructions);
    printf("final state of machine:\n");
    printState(&state);

    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
