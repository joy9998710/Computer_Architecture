/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
/* Define max label number as 1000 */
#define MAXLABELNUM 1000

struct Label{
	char *name;
	int address;
	int value; /* Value that label contains */
	char *label;
};

struct Label* Labels[MAXLABELNUM];
int labelnum;

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int isValidReg(char *);
int RTypeFormat(char *, char *, char *, char *);
int ITypeFormat(char *, char *, char *, char *, int);
int JTypeFormat(char *, char *, char *);
int OTypeFormat(char *);
void initLabel(FILE *, char *, char *, char *, char *, char *);
int isValidLabel(char *);
int getLabelAddress(char *);
int getLabelValue(char *);

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
	int pc = 0;
	initLabel(inFilePtr, label, opcode, arg0, arg1, arg2);

	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		int machine_code = 0;
		/* R-Type */
		if(!strcmp(opcode, "add") || !strcmp(opcode, "nor")){
			machine_code = RTypeFormat(opcode, arg0, arg1, arg2);
		}
		/* I-Type */
		else if(!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")){
			machine_code = ITypeFormat(opcode, arg0, arg1, arg2, pc);
		}
		/* J-Type */
		else if(!strcmp(opcode, "jalr")){
			machine_code = JTypeFormat(opcode, arg0, arg1);
		}
		/* O-Type */
		else if(!strcmp(opcode, "halt") || !strcmp(opcode, "noop")){
			machine_code = OTypeFormat(opcode);
		}
		/* */
		else if(!strcmp(opcode, ".fill")){
			if(isNumber(arg0)){
				machine_code = atoi(arg0);
			}
			else{
				machine_code = getLabelAddress(arg0);
			}
		}
		else{
			printf("Undefined opcode\n");
			printf("%s\n", opcode);
			exit(1);
		}
		fprintf(outFilePtr, "%d\n", machine_code);
		printf("(address %d): %d (hex 0x%x)\n", pc, machine_code, machine_code);
		pc++;
	}

	for(int i = 0; i < MAXLABELNUM; i++){
		if(Labels[i]->name != NULL){
			free(Labels[i]->name);
		}
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
	if(!isNumber(reg)){
		return 0;
	}
	int regs = atoi(reg);
	if(regs < 0 || regs > 7){
		return 0;
	}
	return 1;
}

/* R type format instruction to machine code */
/* and | nor */
int RTypeFormat(char *opcode, char *reg1, char *reg2, char *destReg){
	int machine_code = 0;
	if(!(isValidReg(reg1)) || !(isValidReg(reg2))){
		printf("Register is not valid\n");
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

	/* reg1 21-19 bits */
	int temp = atoi(reg1);
	machine_code |= (temp << 19);
	/* reg2 18-16 bits */
	temp = atoi(reg2);
	machine_code |= (temp << 16);
	/* destReg 2-0 bits */
	temp = atoi(destReg);
	machine_code |= temp;

	return machine_code;
}

int ITypeFormat(char *opcode, char *reg1, char *reg2, char *offsetField, int pc){
	int machine_code = 0;
	if(!(isValidReg(reg1)) || !(isValidReg(reg2))){
		printf("Register is not valid\n");
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

	/* reg1 : 21-19 bits */
	int temp = atoi(reg1);
	machine_code |= (temp << 19);
	/* reg2 : 18-16 bits */
	temp = atoi(reg2);
	machine_code |= (temp << 16);

	/* offset */

	temp = 0;
	if(isNumber(offsetField)){
		temp = atoi(offsetField);

		/* Check valid offset */
		if(temp > 32767 || temp < -32768){
			printf("Offset out of range\n");
			exit(1);
		}
	}
	else{
		if(!strcmp(opcode, "lw")){
			temp = getLabelAddress(offsetField);
		}
		else if(!strcmp(opcode, "sw")){
			temp = getLabelAddress(offsetField);
		}
		else if(!strcmp(opcode, "beq")){
			temp = getLabelAddress(offsetField);
			temp -= pc + 1;
		}
		/* check valid offset */
		if(temp > 32767 || temp < -3268){
			printf("Offset out of range\n");
			exit(1);
		}
	}
	temp &= 0xFFFF;
	machine_code |= temp;
	return machine_code;
}

/* J type format instruction to machine code */
/* jalr */
int JTypeFormat(char *opcode, char *reg1, char *reg2){
	int machine_code = 0;
	if(!(isValidReg(reg1)) || !(isValidReg(reg2))){
		printf("Register is not valid\n");
		exit(1);
	}
	/* Opcode: 24-22 bits */
	/* jalr: 101 */
	if(!strcmp(opcode, "jalr")){
		machine_code |= (5 << 22);
	}

	/* reg1 : 21019 bits */
	int temp = atoi(reg1);
	machine_code |= (temp << 19);
	/* reg2: 18-16 bits */
	temp = atoi(reg2);
	machine_code |= (temp << 16);

	return machine_code;
}

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

/* Getting all the labels inthe code */
void initLabel(FILE *inFilePtr, char *label, char *opcode, char * arg0, char *arg1, char * arg2){
	//memory allocation for Label array
	for(int i = 0; i < MAXLABELNUM; i++){
		Labels[i] = (struct Label*)malloc(sizeof(struct Label));
	}
	/* Initialize the Labels */
	for(int i = 0; i < MAXLABELNUM; i++){
		Labels[i]->name = NULL;
		Labels[i]->label = NULL;
	}
	labelnum = 0;
	/* program counter */
	int pc = 0;

	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		/* it ehre is no label in current line continue */
		if(!strcmp(label, "")){
			pc++;
			continue;
		}
		/* check wheter it is valid or not */
		if(!isValidLabel(label)){
			printf("Invalid Label\n");
			exit(1);
		}
		/* Check wheter the label already exist */
		for(int i = 0; i < labelnum; i++){
			if(!strcmp(label, Labels[i]->name)){
				printf("Label already exist\n");
				exit(1);
			}
		}
		
		Labels[labelnum]->name = (char*)malloc(MAXLINELENGTH*sizeof(char));
		strcpy(Labels[labelnum]->name, label);
		Labels[labelnum]->address = pc++;

		/* if .fill */
		if(!strcmp(opcode, ".fill")){
			/* if the argument is a number then the value of that label will be number */
			if(isNumber(arg0)){
				//To check overflow use long long as this is 64 bits
				long long check = atoll(arg0);
				if(check > 2147483647 || check < -2147483648){
					printf(".fill out of range\n");
					exit(1);
				}
				Labels[labelnum]->value = atoi(arg0);
			}
			else{
				Labels[labelnum]->label = (char*)malloc(MAXLINELENGTH*sizeof(char));
				strcpy(Labels[labelnum]->label, arg0);
			}
		}
		labelnum++;
	}
	/* Rewind after initializing labels */
	for(int i = 0; i < labelnum; i++){
		if(Labels[i]->label != NULL){
			Labels[i]->value = getLabelAddress(Labels[i]->label);
			free(Labels[i]->label);
		}
	}
	rewind(inFilePtr);
}

int isValidLabel(char *name){
	/* Maximum length of label is 6 */
	if(strlen(name) > 6){
		return 0;
	}
	/* Starts with number */
	if(isNumber(&name[0])){
		return 0;
	}
	/* Consist of only letters and numbers */
	for(int i = 0; i < strlen(name); i++){
		/* both not number and letter */
		if(!(isNumber(&name[i]))){
			if((name[i] < 'a' || name[i] > 'z') && (name[i] < 'A' || name[i] > 'Z')){
				return 0;
			}
		}
	}
	return 1;
}

int getLabelAddress(char *name){
	for(int i = 0; i < labelnum; i++){
		if(!strcmp(Labels[i]->name, name)){
			return Labels[i]->address;
		}
	}
	printf("Label not found\n");
	exit(1);
}

int getLabelValue(char *name){
	for(int i = 0; i < labelnum; i++){
		if(!strcmp(Labels[i]->name, name)){
			return Labels[i]->value;
		}
	}
	printf("Label not found\n");
	exit(1);
}