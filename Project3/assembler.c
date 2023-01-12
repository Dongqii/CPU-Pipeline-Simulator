/**
 * Project 1 
 * Assembler code fragment for LC-2K 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAXLINELENGTH 1000
#define MAXNUMLABEL 65536
#define MAXLABELL 7 //need to include the null character at the end

#define ADD 0
#define NOR 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7

#pragma warning(disable:4996)

//creating a array that store 

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int AddNor(const char*, const char*, const char*, const char*); //for add Nor
int LwSwBeq(const char*, const char*, const char*, const char*, const char* , char[MAXNUMLABEL][7], int); //for Lw Sw Beq
//int LwSwBeq(const char* label, const char* opcode, const char* arg0, const char* arg1, const char* arg2, char labels[MAXNUMLABEL][7], int curr_add) {
int jalr(const char*, const char*, const char*); //for jair 
int HaltNoop(const char*); //halt/noop
int fill(const char* , char[MAXNUMLABEL][7]); //fill
int findLabelAddress(const char*, char[MAXNUMLABEL][7]);// returns the address of the label
int duplicates(const char*, int, char[MAXNUMLABEL][7]); //0 if no repeat, 1 if has a repeat
int masking(int, int); 
//int masking(int, int);


//int labelConversion(char*); //converting label to is address

//used to save lables 


int
main(int argc, char *argv[])
{
    //setting variables
    char labels[MAXNUMLABEL][MAXLABELL]; //maximum numble of words, max label length, use to save label name in first pass
    int curr_add = 0; 
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */



    //Begining of first pass, read labels. 
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {/* reached end of file */
        //TODO: add test to vertify label is valid
            //TODO: add test to test label have approprate length
        if (strcmp(label, "") && strlen(label) >= MAXLABELL) {
            exit(1);
        }

        char temp[1000];
            //add test to test first is a letter 
        if (strcmp(label, "") && !sscanf(label, "%[a-zA-Z]", temp)) {
            exit(1);
        }

        if (strcmp(label, "")) {
            sscanf(label, "%[0-9a-zA-Z]", temp);
            if (strcmp(label, temp) != 0) {//if the two string is not equal, exit 
                exit(1);
            }
        }

            //TODO: add test to test label only contains letter and numbers
        if (strcmp(label, "") && duplicates(label, curr_add, labels) == 1) { //check duplicate labels 
            exit(1); 
        }


        //read labels and save their index
        strcpy(labels[curr_add], label);
        ++curr_add; 
    }
    /* rewind the file ptr to start reading from the beginning of the file */
    rewind(inFilePtr);
    curr_add = 0; 
    //end of first pass


    //start of second pass
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {        /* reached end of file */

        //TODO: check unrecognized opcodes 

        int machine_num = 0;

        if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")) {
            machine_num = AddNor(opcode, arg0, arg1, arg2);
        }
        else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
            machine_num = LwSwBeq(label, opcode, arg0, arg1, arg2, labels, curr_add);
        }
        else if (!strcmp(opcode, "jalr")) {
            machine_num = jalr(opcode, arg0, arg1);
        }
        else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")) {
            machine_num = HaltNoop(opcode);
        }
        else if (!strcmp(opcode, ".fill")) {
            machine_num = fill(arg0, labels); 
        }
        else {
            exit(1); //checked unrecognized opcode 
        }

        fprintf(outFilePtr, "%d\n", machine_num);//%d\n nothing printed or print decimal numbers 
        //fprintf(outFilePtr, "\n");//adding a newline character
        curr_add++;
    } 
    exit(0); //reached end of file 
    return 0;
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if successfully read
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
        char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
        /* reached end of file */
        return(0);
    }
    /*
    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        //printf("error: line too long\n");
        //exit(1);
    }
    /* is there a label? */
    char *ptr = line;
    if (sscanf(ptr, "%[^\t\n\r ]", label)) {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int AddNor(const char* opcode, const char* arg0, const char* arg1, const char* arg2) {
    int machine_code;
    if (!strcmp("add", opcode)) {
        machine_code = (ADD << 22); //0b represent binary
    }
    else {
        machine_code = (NOR << 22);
    }

    machine_code |= (atoi(arg0) << 19); //put register 0 in place
    machine_code |= (atoi(arg1) << 16); //put register 1 in place
    machine_code |=  atoi(arg2); //put destination in place
    //atoi() converts char to int
    return machine_code;
}

int LwSwBeq(const char* label, const char* opcode, const char* arg0, const char* arg1, const char* arg2, char labels[MAXNUMLABEL][7], int curr_add) {
    int machine_code = 0; 
    int offset = 0; 
    if (!strcmp("beq", opcode) ) {// beq
        machine_code = (BEQ << 22);
        machine_code |= (atoi(arg0) << 19); //put register 0 in place
        machine_code |= (atoi(arg1) << 16); //put register 1 in place
        if (isNumber(arg2)) {
            offset = atoi(arg2);
        }
        else {
            if (findLabelAddress(arg2, labels) == -1) {
                exit(1); 
            }
            offset = findLabelAddress(arg2, labels) - curr_add - 1; 
        }
    }
    else { //lw & sw
        if (!strcmp("lw", opcode)) {
            machine_code = (LW << 22);
        }
        if (!strcmp("sw", opcode)) {
            machine_code = SW << 22; 
        }
        machine_code |= (atoi(arg0) << 19); //put register 0 in place
        machine_code |= (atoi(arg1) << 16); //put register 1 in place
        if (isNumber(arg2)) { //the offset is a immediate value
            offset = atoi(arg2);
        }
        else {//the offset is a label
            if (findLabelAddress(arg2, labels) == -1) {
                exit(1); 
            }
            offset = findLabelAddress(arg2, labels); //implement findLabelAddress later 
        }
    }//else
    //do masking on offset and check is in the range. (with &)
    if (offset < -32768 || offset > 32767) { //the offset is too large
            exit(1);
    }
    offset = masking(offset, 16);
    machine_code += offset; 
    return machine_code; 
}//lwswbeq

int jalr(const char* opcode, const char* arg0, const char* arg1) {
    int machine_code = 0; 
    machine_code = (JALR << 22);
    machine_code |= (atoi(arg0) << 19); //put register 0 in place
    machine_code |= (atoi(arg1) << 16); //put register 1 in place
    return machine_code;
}

int HaltNoop(const char* opcode) {
    int machine_code = 0;
    if (!strcmp("halt", opcode)) {
        machine_code = (HALT << 22);
    }
    else if (!strcmp("noop", opcode)) {
        machine_code = (NOOP << 22);
    }
    return machine_code;
}

int fill(const char* arg0, char labels[MAXNUMLABEL][7]) {
    int machine_code = 0;
    if (isNumber(arg0)) {
        machine_code = atoi(arg0);
    }
    else {
        machine_code = findLabelAddress(arg0, labels);
    }
    return machine_code; 
}


int findLabelAddress(const char* arg2, char labels[MAXNUMLABEL][7]) {
    for (int i = 0; i < MAXNUMLABEL; i++) {
        if (strcmp(labels[i], arg2) == 0) {
            return i;
        }
    }
    return -1; 
}

int masking(int offset, int digit) { 
    int mask = pow(2, digit) - 1; 
    if (offset > mask) { //offset dont fit in 16 bits. 
        exit(1);
    }
    mask = mask & offset;
    return mask; 
}

int duplicates(const char* label, int curr_add, char labels[MAXNUMLABEL][7]) {
    for (int i = 0; i < MAXNUMLABEL; i++) {
        if (!strcmp(labels[i], label) && i != curr_add) {
            return 1; 
        }
    }
    return 0; 
}

//int chopping16(int offset) {
  //  int 

