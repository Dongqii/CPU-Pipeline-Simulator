/**
 * EECS 370 Project 3
 * Pipeline Simulator
 *
 * This fragment should be used to modify your project 1 simulator to simulator
 * a pipeline
 *
 * Make sure *not* to modify printState or any of the associated functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5 /* JALR will not implemented for Project 3 */
#define HALT 6
#define NOOP 7

#define NOOPINSTRUCTION 0x1c00000
#pragma warning(disable:4996)

typedef struct IFIDStruct {
	int instr;
	int pcPlus1;
} IFIDType;

typedef struct IDEXStruct {
	int instr;
	int pcPlus1;
	int readRegA;
	int readRegB;
	int offset;
    int dest; //added destination
} IDEXType;

typedef struct EXMEMStruct {
	int instr;
	int branchTarget;
	int aluResult;
	int readRegB;
    int dest; //added destination
} EXMEMType;

typedef struct MEMWBStruct {
	int instr;
	int writeData;
    int dest; //added destination
} MEMWBType;

typedef struct WBENDStruct {
	int instr;
	int writeData;
    int dest; //added destination
} WBENDType;

typedef struct stateStruct {
	int pc;
	int instrMem[NUMMEMORY];
	int dataMem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
	IFIDType IFID;
	IDEXType IDEX;
	EXMEMType EXMEM;
	MEMWBType MEMWB;
	WBENDType WBEND;
	int cycles; /* number of cycles run so far */
} stateType;

int
field0(int instruction)
{
    return( (instruction>>19) & 0x7);
}

int
field1(int instruction)
{
    return( (instruction>>16) & 0x7);
}

int
field2(int instruction)
{
    return(instruction & 0xFFFF);
}

int
opcode(int instruction)
{
    return(instruction>>22);
}

void
printInstruction(int instr)
{

    char opcodeString[10];

    if (opcode(instr) == ADD) {
        strcpy(opcodeString, "add");
    } else if (opcode(instr) == NOR) {
        strcpy(opcodeString, "nor");
    } else if (opcode(instr) == LW) {
        strcpy(opcodeString, "lw");
    } else if (opcode(instr) == SW) {
        strcpy(opcodeString, "sw");
    } else if (opcode(instr) == BEQ) {
        strcpy(opcodeString, "beq");
    } else if (opcode(instr) == JALR) {
        strcpy(opcodeString, "jalr");
    } else if (opcode(instr) == HALT) {
        strcpy(opcodeString, "halt");
    } else if (opcode(instr) == NOOP) {
        strcpy(opcodeString, "noop");
    } else {
        strcpy(opcodeString, "data");
    }
    printf("%s %d %d %d\n", opcodeString, field0(instr), field1(instr),
        field2(instr));
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate before cycle %d starts\n", statePtr->cycles);
    printf("\tpc %d\n", statePtr->pc);

    printf("\tdata memory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
        printf("\t\tdataMem[ %d ] %d\n", i, statePtr->dataMem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("\tIFID:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IFID.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IFID.pcPlus1);
    printf("\tIDEX:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->IDEX.instr);
    printf("\t\tpcPlus1 %d\n", statePtr->IDEX.pcPlus1);
    printf("\t\treadRegA %d\n", statePtr->IDEX.readRegA);
    printf("\t\treadRegB %d\n", statePtr->IDEX.readRegB);
    printf("\t\toffset %d\n", statePtr->IDEX.offset);
    printf("\tEXMEM:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->EXMEM.instr);
    printf("\t\tbranchTarget %d\n", statePtr->EXMEM.branchTarget);
    printf("\t\taluResult %d\n", statePtr->EXMEM.aluResult);
    printf("\t\treadRegB %d\n", statePtr->EXMEM.readRegB);
    printf("\tMEMWB:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->MEMWB.instr);
    printf("\t\twriteData %d\n", statePtr->MEMWB.writeData);
    printf("\tWBEND:\n");
    printf("\t\tinstruction ");
    printInstruction(statePtr->WBEND.instr);
    printf("\t\twriteData %d\n", statePtr->WBEND.writeData);
}


//start implementing data hazard
//first investigater how many situation need intervine
//then construct data write back
void dataForward(stateType* state, stateType* newState, int *curRegA, int *curRegB) {
    //dependency 2 cycle away
    if (opcode(state->WBEND.instr) == LW || opcode(state->WBEND.instr) == ADD || opcode(state->WBEND.instr) == NOR) {//instruction that write back
        if (state->WBEND.dest == field0(state->IDEX.instr) &&
            (opcode(state->IDEX.instr) == ADD || opcode(state->IDEX.instr) == NOR
                || opcode(state->IDEX.instr) == LW || opcode(state->IDEX.instr) == SW || opcode(state->IDEX.instr) == BEQ)) {//RegA has a data dependency and needs to get rewritten 
            *curRegA = state->WBEND.writeData;
        }
        if (state->WBEND.dest == field1(state->IDEX.instr) &&
            (opcode(state->IDEX.instr) == ADD || opcode(state->IDEX.instr) == NOR
                || opcode(state->IDEX.instr) == SW || opcode(state->IDEX.instr) == BEQ)) { //RegB has a data dependency need over written
            *curRegB = state->WBEND.writeData;
        }
    }
    //dependency 1 cycles away
    if (opcode(state->MEMWB.instr) == LW || opcode(state->MEMWB.instr) == ADD || opcode(state->MEMWB.instr) == NOR) {//instruction that write back
        if (state->MEMWB.dest == field0(state->IDEX.instr) &&
            (opcode(state->IDEX.instr) == ADD || opcode(state->IDEX.instr) == NOR
                || opcode(state->IDEX.instr) == LW || opcode(state->IDEX.instr) == SW || opcode(state->IDEX.instr) == BEQ)) {//RegA has a data dependency and needs to get rewritten 
            *curRegA = state->MEMWB.writeData;
        }
        if (state->MEMWB.dest == field1(state->IDEX.instr) &&
            (opcode(state->IDEX.instr) == ADD || opcode(state->IDEX.instr) == NOR
                || opcode(state->IDEX.instr) == SW || opcode(state->IDEX.instr) == BEQ)) { //RegB has a data dependency need over written
            *curRegB = state->MEMWB.writeData;
        }
    }
    //dependency 0 cycles away(Lw not reachable yet) 
    if (opcode(state->EXMEM.instr) == ADD || opcode(state->EXMEM.instr) == NOR) {
        if (state->EXMEM.dest == field0(state->IDEX.instr) &&
            (opcode(state->IDEX.instr) == ADD || opcode(state->IDEX.instr) == NOR
                || opcode(state->IDEX.instr) == LW || opcode(state->IDEX.instr) == SW || opcode(state->IDEX.instr) == BEQ)) {//RegA has a data dependency and needs to get rewritten 
            *curRegA = state->EXMEM.aluResult;
        }
        if (state->EXMEM.dest == field1(state->IDEX.instr) &&
            (opcode(state->IDEX.instr) == ADD || opcode(state->IDEX.instr) == NOR
                || opcode(state->IDEX.instr) == SW || opcode(state->IDEX.instr) == BEQ)) { //RegB has a data dependency need over written
            *curRegB = state->EXMEM.aluResult;
        }
    }
}

int convert_32(int num) {//turn the offset field(16bits) into 32bits number
    if (num & (1 << 15)) {
        num -= (1 << 16);
    }
    return num ;
}
//start implementing control hazard

stateType state, newState; //creating state, new state

int main(int argc, char* argv[])
{
    //creating variables, read input section, need modify 

    char line[MAXLINELENGTH];
    //stateType state, newState; //creating state, new state
        //initialize the pc and all registers to zero, insert noop into all instruction field in all pipeline registers
    state.pc = 0;
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;
    }
    state.IFID.instr = NOOPINSTRUCTION;
    state.IDEX.instr = NOOPINSTRUCTION;
    state.EXMEM.instr = NOOPINSTRUCTION;
    state.MEMWB.instr = NOOPINSTRUCTION;
    state.WBEND.instr = NOOPINSTRUCTION;


    FILE* filePtr;

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

    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
        state.numMemory++) {

        if (sscanf(line, "%d", state.instrMem + state.numMemory) != 1) {//savedata into instrMem
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        sscanf(line, "%d", state.dataMem + state.numMemory); //save data into dataMem

        printf("memory[%d]=%d\n", state.numMemory, state.instrMem[state.numMemory]);
    }
    printf("%d memory words\n", state.numMemory);
    printf("\tinstruction memory:\n");
    for (int i = 0; i < state.numMemory; i++) {
        printf("\t\tinstrMem[ %d ] ", i);
        printInstruction(state.instrMem[i]);
    }
    //end of read input

    //the run() project 3 main loop
    while (1) {

        printState(&state);

        /* check for halt */
        if (opcode(state.MEMWB.instr) == HALT) {
            printf("machine halted\n");
            printf("total of %d cycles executed\n", state.cycles);
            exit(0);
        }

        newState = state;
        newState.cycles++;



        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //there's a but in IF and ID check it tommorrow(most likely in IF)
        //It is also possible that the bug is in control hazard(leading to faulty jump thus false IF)

        /* --------------------- IF stage --------------------- */

        //Check data hazard
        //this is the case which we need to insert noop 
        if (opcode(state.IDEX.instr) == LW) {
            if ((opcode(state.IFID.instr) == ADD || opcode(state.IFID.instr) == NOR || opcode(state.IFID.instr) == BEQ || opcode(state.IFID.instr) == SW) &&
                (field0(state.IFID.instr) == field1(state.IDEX.instr) || field1(state.IFID.instr) == field1(state.IDEX.instr))) {
                newState.IDEX.instr = NOOPINSTRUCTION;
            }
            else if (opcode(state.IFID.instr) == LW && field0(state.IFID.instr) == field1(state.IDEX.instr)) {// IFID&&LW and field0(regA) is equal to IDEX field1
                newState.IDEX.instr = NOOPINSTRUCTION; //!!make sure this is the case
            }
            /*if ((opcode(state.IFID.instr) == ADD || opcode(state.IFID.instr) == NOR || opcode(state.IFID.instr) == BEQ || opcode(state.IFID.instr) == SW) &&
                (field0(state.IFID.instr) == field1(state.IDEX.instr) || field1(state.IFID.instr) == field1(state.IDEX.instr))) {
                newState.IDEX.instr = NOOPINSTRUCTION;
            }
            else if (opcode(state.IFID.instr) == LW && 
                (field0(state.IFID.instr) == field1(state.IDEX.instr) || field1(state.IFID.instr) == field1(state.IDEX.instr))) {// IFID&&LW and field0(regA) is equal to IDEX field1
                newState.IDEX.instr = NOOPINSTRUCTION; //!!make sure this is the case
            }*/
            else {//regular fetch
                //update pipeline register in new state
                newState.IFID.instr = state.instrMem[state.pc];
                newState.IFID.pcPlus1 = state.pc + 1;
                //update pc in new state
                newState.pc = state.pc + 1; 
                newState.IDEX.instr = state.IFID.instr;
            }
        }
        else {
            //regular fetch
            newState.IFID.instr = state.instrMem[state.pc];
            newState.IFID.pcPlus1 = state.pc + 1;
            newState.pc = state.pc + 1;
            newState.IDEX.instr = state.IFID.instr;
        }


        /* --------------------- ID stage --------------------- */
        //update pipeline register in new state
        //newState.IDEX.instr = state.IFID.instr;
        newState.IDEX.pcPlus1 = state.IFID.pcPlus1;
        newState.IDEX.readRegA = state.reg[field0(state.IFID.instr)];
        newState.IDEX.readRegB = state.reg[field1(state.IFID.instr)];
        newState.IDEX.offset = convert_32(field2(state.IFID.instr));
        //destination
        if (opcode(state.IFID.instr) == ADD || opcode(state.IFID.instr) == NOR) {
            newState.IDEX.dest = field2(state.IFID.instr); //dest is the dest reg
        }
        else if (opcode(state.IFID.instr) == LW) {
            newState.IDEX.dest = field1(state.IFID.instr); //dest is regb
        }



        /* --------------------- EX stage --------------------- */
        //update pipeline register in new state
        newState.EXMEM.instr = state.IDEX.instr;
        newState.EXMEM.branchTarget = state.IDEX.pcPlus1 + state.IDEX.offset;//branchTarget
        newState.EXMEM.dest = state.IDEX.dest;
        //here you go data hazard!
        //pick the closest! 


        int curRegA = state.IDEX.readRegA;//creating temporary registers to hold updated value 
        int curRegB = state.IDEX.readRegB;
        dataForward(&state, &newState, &curRegA, &curRegB); 

        //aluResult
        if (opcode(state.IDEX.instr) == ADD) {
            newState.EXMEM.aluResult = curRegB + curRegA;
        }
        else if (opcode(state.IDEX.instr) == NOR) {
            newState.EXMEM.aluResult = ~(curRegB | curRegA);
        }
        else if (opcode(state.IDEX.instr) == LW || opcode(state.IDEX.instr) == SW){
            newState.EXMEM.aluResult = curRegA + state.IDEX.offset;
        }
        else if (opcode(state.IDEX.instr) == BEQ) {//if this result equal to 0, branch 
            newState.EXMEM.aluResult = curRegB - curRegA;
        }
        else { //noop & halt 
        }

        newState.EXMEM.readRegB = curRegB;//readRegB



        /* --------------------- MEM stage --------------------- */
        //update pipeline register in new state
        newState.MEMWB.instr = state.EXMEM.instr;
        newState.MEMWB.dest = state.EXMEM.dest;
        //writeData
        //only lw, add, nor need data write back
        if (opcode(newState.MEMWB.instr) != NOOP) {
            if (opcode(state.EXMEM.instr) == ADD || opcode(state.EXMEM.instr) == NOR) {
                newState.MEMWB.writeData = state.EXMEM.aluResult;
            }
            else if (opcode(state.EXMEM.instr) == LW) {
                newState.MEMWB.writeData = state.dataMem[state.EXMEM.aluResult];
            }
            else if (opcode(state.EXMEM.instr) == BEQ) {
                if (state.EXMEM.aluResult == 0) {
                    newState.IFID.instr = NOOPINSTRUCTION;
                    newState.IDEX.instr = NOOPINSTRUCTION;
                    newState.EXMEM.instr = NOOPINSTRUCTION;
                    newState.pc = state.EXMEM.branchTarget;
                }
            }

            if (opcode(state.EXMEM.instr) == SW) {
                newState.MEMWB.writeData = state.EXMEM.aluResult;
                newState.dataMem[state.EXMEM.aluResult] = state.EXMEM.readRegB;
            }
        }


        /* --------------------- WB stage --------------------- */
        //update pipeline register in new state
        newState.WBEND.instr = state.MEMWB.instr;
        newState.WBEND.dest = state.MEMWB.dest;
        //writeData
        newState.WBEND.writeData = state.MEMWB.writeData; 

        //register write back 
        if (opcode(state.MEMWB.instr) == ADD || opcode(state.MEMWB.instr) == NOR || opcode(state.MEMWB.instr) == LW) {
            newState.reg[state.MEMWB.dest] = state.MEMWB.writeData; //precalculated            
        }



        state = newState; /* this is the last statement before end of the loop.
            It marks the end of the cycle and updates the
            current state with the values calculated in this
            cycle */
    }
}

