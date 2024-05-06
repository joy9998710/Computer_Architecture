/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
/* Define max label number as 1000 As this is a small program*/
#define MAXLABELNUM 1000

/* label container */
struct Label{
	char *name;
	int address;
	char *label;
	int value;
};

struct Label* Labels[MAXLABELNUM];

/* label num */
int labelnum;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int isValidReg(char *);
int RTypeFormat(char *, char *, char *, char *);
int ITypeFormat(char *, char *, char *, char *, int);
int JTypeFormat(char *, char *, char *);
int OTypeFormat(char *);
int isValidLabel(char *);
int isLetter(char *);
void initlabel(FILE *, char *, char *, char *, char *, char *);
int isValidNumber(long long);
int getLabelValue(char *);
int getLabelAddress(char *);

int main(int argc, char *argv[]) 
{
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

	/* TODO: Phase-1 label calculation */
	/* Initialize Labels */
	/* Program counter */
	int pc = 0;
	initlabel(inFilePtr, label, opcode, arg0, arg1, arg2);

	/* TODO: Phase-2 generate machine codes to outfile */
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		int machine_code;
		/* if opcode is R-Type */
		if(!strcmp(opcode, "add") || ! strcmp(opcode, "nor")){
			machine_code = RTypeFormat(opcode, arg0, arg1, arg2);
		}
		/* if opcode is I-Type */
		else if(!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")){
			machine_code = ITypeFormat(opcode, arg0, arg1, arg2, pc);
		}
		/* if opcode is J-Type */
		else if(!strcmp(opcode, "jalr")){
			machine_code = JTypeFormat(opcode, arg0, arg1);
		}
		/* if opcode is O-Type */
		else if(!strcmp(opcode, "halt") || !strcmp(opcode, "noop")){
			machine_code = OTypeFormat(opcode);
		}
		else if(!strcmp(opcode, ".fill")){
			if(isNumber(arg0)){
				machine_code = atoi(arg0);
			}
			else{
				machine_code = getLabelValue(label);
			}
		}
		else{
			printf("Unrecognized opcode");
			exit(1);
		}
		fprintf(outFilePtr, "(address %d): %d (hex 0x%x)\n", pc, machine_code, machine_code);
		pc++;
	}

	for(int i = 0; i < MAXLABELNUM; i++){
		free(Labels[i]->name);
		free(Labels[i]->label);
		free(Labels[i]);
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

int isValidReg(char *reg){
	/* return 1 if register is valid*/
	if(!isNumber(reg)){
		return 0;
	}
	int regs = atoi(reg);
	if(regs < 0 || regs > 7){
		return 0;
	}
	return 1;
}

/* RType format instruction to machine code */
/* and, nor */
int RTypeFormat(char *opcode, char *reg0, char *reg1, char *destReg){
	/* As the machine code is 4 byte declare it as an int value*/
	int machine_code = 0;
	/* Check if registers are valid*/
	if(!(isValidReg(reg0)) || !(isValidReg(reg1)) || !(isValidReg(destReg))){
		printf("Register is not valid");
		exit(1);
	}
	/* Opcode: 24-22 bits */
	/* add 000 */
	if(!strcmp(opcode, "add")){
		machine_code = (0 << 22);
	}
	/* nor 001 */
	else if(!strcmp(opcode, "nor")){
		machine_code = (1 << 22);
	}

	/* reg 0 21-19 bits */
	int temp = atoi(reg0);
	machine_code |= (temp << 19);
	/* reg 1 18-16 bits */
	temp = atoi(reg1);
	machine_code |= (temp << 16);
	/* destReg 2-0 bits*/
	temp = atoi(destReg);
	machine_code |= (temp);

	return machine_code;
}

/* I type format instruction to machine code */
/* lw, sw, beq */
int ITypeFormat(char *opcode, char *reg0, char *reg1, char *offsetField, int pc){
	int machine_code = 0;
	if(!(isValidReg(reg0)) || !(isValidReg(reg1))){
		printf("Register is not valid");
		exit(1);
	}

	/* Opcode: 24-22 bits */
	/* lw: 010 */
	if(!strcmp(opcode, "lw")){
		machine_code = (2 << 22);
	}
	/* sw: 011 */
	else if(!strcmp(opcode, "sw")){
		machine_code = (3 << 22);
	}
	/* beq: 100 */
	else if(!strcmp(opcode, "beq")){
		machine_code = (4 << 22);
	}

	/* reg0 : 21- 19 bits */
	int temp = atoi(reg0);
	machine_code |= (temp << 19);
	/* reg1 : 18-16 bits */
	temp = atoi(reg1);
	machine_code |= (temp << 16);

	temp = 0;
	if(isNumber(offsetField)){
		temp = atoi(offsetField);

		if(temp > 32767 || temp < -32768){
			printf("offset out of range");
			exit(1);
		}
	}
	else{
		temp = getLabelAddress(offsetField);
	}


	if(!strcmp(opcode, "beq") && !isNumber(offsetField)){
		//using offset 
		temp -= pc - 1;
		if(temp > 32787 || temp < -73788){
			printf("offset out of range");
			exit(1);
		}
	}
	/* Address is 16 bits */
	temp &= 0xFFFF;
	machine_code |= temp;

	return machine_code;
}

/* J type format instruction to machine code */
/* jalr */
int JTypeFormat(char *opcode, char *reg0, char *reg1){
	int machine_code = 0;
	if(!(isValidReg(reg0)) || !(isValidReg(reg1))){
		printf("Register is not valid");
		exit(1);
	}
	/* Opcode: 24-22 bits */
	/* jalr : 101 */
	if(!strcmp(opcode, "jalr")){
		machine_code |= (5 << 22);
	}

	/* reg0: 21-19 bits */
	int temp = atoi(reg0);
	machine_code |= (temp << 19);
	/* reg1: 18-16 */
	temp = atoi(reg1);
	machine_code |= (temp << 16);

	return machine_code;
}

/* O type format to machine code */
/* halt, noop */
int OTypeFormat(char *opcode){
	int machine_code = 0;

	/* Opcode: 24-22 bits */
	/* halt: 110 */
	if(!strcmp(opcode, "halt")){
		machine_code = (6 << 22);
	}
	/* noop: 111 */
	else if(!strcmp(opcode, "noop")){
		machine_code = (7 << 22);
	}
	return machine_code;
}

/* Getting all the labels in the code */
void initlabel(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2){
	//memory allocation for Label array
	for(int i = 0; i < MAXLABELNUM; i++){
		Labels[i] = (struct Label*)malloc(sizeof(struct Label));
	}
	/* Initialize the Lables */
	for(int i = 0; i < MAXLABELNUM; i++){
		Labels[i]->name = NULL;
		Labels[i]->label = NULL;
	}
	labelnum = 0;
	/* program counter */
	int pc = 0;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		/* if there is no label in this line continue */
		if(!strcmp(label, "")){
			pc++;
			continue;
		}
		/* check whether it is valid or not */
		if(!isValidLabel(label)){
			printf("Invalid Label");
			exit(1);
		}
		/* Check whether the label already exist*/
		for(int i = 0; i < labelnum; i++){
			if(!strcmp(label, Labels[i]->name)){
				printf("Label already exist");
				exit(1);
			}
		}
		/* After passing all the validation test insert it */
		Labels[labelnum]->name = (char*)malloc(MAXLINELENGTH*sizeof(char));
		strcpy(Labels[labelnum]->name, label);
		Labels[labelnum]->address = pc++;

		/* if it is fill value then we have to store value in label */
		if(!strcmp(opcode, ".fill")){
			if(isNumber(arg0)){
				long long check = atoi(arg0);
				if(isValidNumber(check)){
					Labels[labelnum]->value = atoi(arg0);
				}
				else{
					printf("Value out of range");
					exit(1);
				}
			}
			else{
				Labels[labelnum]->label = (char*)malloc(MAXLINELENGTH*sizeof(char));
				strcpy(Labels[labelnum]->label, arg0);
			}
		}
		labelnum++;
	}
	/* Rewind after initializing labels */
	rewind(inFilePtr);
}


int isValidLabel(char *name){
	/* Maximum length of label is 6 */
	if(strlen(name) > 6){
		return 0;
	}
	/* Starts with letter*/
	if(!isLetter(&name[0])){
		return 0;
	}
	/* Consist of not only letter and number */
	for(int i = 0; i < strlen(name); i++){
		if(!(isNumber(&name[i])) && !(isLetter(&name[i]))){
			return 0;
		}
	}
	return 1;
}

int isLetter(char *alphabet){
	if(*alphabet >= 'a' && *alphabet <= 'z'){
		return 1;
	}
	if(*alphabet >= 'A' && *alphabet <= 'Z'){
		return 1;
	}
	return 0;
}

/* for the arguments in .fill instruction */
/* To check that value is in the range, we need more bits than 32 */
int isValidNumber(long long num){
	if(num > 2147483647 || num < -2147483648){
		return 0;
	}
	return 1;
}

int getLabelValue(char *label){
	for(int i = 0; i < labelnum; i++){
		if(!strcmp(Labels[i]->name, label)){
			if(Labels[i]->label == NULL){
				return Labels[i]->value;
			}
			else{
				return getLabelAddress(Labels[i]->label);
			}
		}
	}
	printf("label not foudn");
	exit(1);
}

int getLabelAddress(char *label){
	for(int i = 0; i < labelnum; i++){
		if(!strcmp(Labels[i]->name, label)){
			return Labels[i]->address;
		}
	}
	printf("label not found");
	exit(1);
}



