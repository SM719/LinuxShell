/*
 ============================================================================
 Name        : eece315_project1.c
 Author      : L2C - C4
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
//#include <string.h>
//#include <unistd.h>

static const char _env_FPath[]= "/etc/environment";//UBUNTU 12.10 Path

char* getFile(char* fileName);
char* getEnv(int* size);
bool setEnv(char* path);

//Struct to pass args for our shell commands
struct command_t { char *name;
	int argc;
	char *argv[];
};

int main(void) {
	//Simple Test to ensure program started
	puts("Shell Started...");

//Variables
	int* pathSize=malloc(sizeof(int));

	char* env[(int) &pathSize];
	//char* env[]=getEnv();
	char* input=(char*) malloc(255);

//Testing
	if(env==NULL){
		puts("getEnv failed");
	}else{

		puts(env);
	}

//Code
	//puts(getEnv());//Prints ENV

	//Shell Loop

	//for(;;){
		scanf("%s", input);
		if(input!=NULL){
			puts(input);
		}

		getchar();
		getchar();
	//}*/
//Clean-up

	return EXIT_SUCCESS;
}
/*
 * Assumes one entry per line
 */
char* getEnv(int* size){
	char* string=getFile(_env_FPath);
	int variablesCount=0;
	int currentVariableIndex=0;
	int nextVariableIndex[sizeof(string)];

	//Find number of env variables
	for(int index=0; index<sizeof(string);index++){
		if(&string[index]=='\n'){
			nextVariableIndex[variablesCount]=index+1;//Add the start of the next string
			variablesCount++;
		}
	}

	char* result[variablesCount];
	//Move Variables into Array
	for(int index=0; index<variablesCount;index++){
		//TODO
		result[index]=malloc((nextVariableIndex[index]-2)-startingIndex);
	}
	return result;
}
/*
char* getEnv(int* size){
	struct stat fileStats;
	FILE* file;

	if(stat(_env_FPath, &fileStats)==-1){
		fprintf(stderr, "Error opening etc/environment");
		exit(1);
	}

	char* result[5];

	file=fopen(_env_FPath,"r");
	if(file!=NULL){
		fscanf(file, "%s", result);
		if(fclose(file)==EOF){
			fprintf(stderr, "Error closing etc/environment");
			exit(1);
		}else
			fprintf(stderr, "Close filestream Successful\n");
	}else{
		fprintf(stderr, "Error opening etc/environment");
		exit(1);
	}

//Clean-up
	return result;
}
*/
bool setEnv(char* path){
	bool successful=false;
	FILE* file;

	file=fopen(_env_FPath,"w");
		if(file!=NULL){
			//fprintf(file, "%s", result);
			if(fclose(file)==EOF){
				fprintf(stderr, "Error closing etc/environment");
				exit(1);
			}else
				fprintf(stderr, "Close filestream Successful\n");
		}else{
			fprintf(stderr, "Error opening etc/environment");
			exit(1);
		}
	return successful;
}
char* getFile(char* fileName){
	char *result = NULL;
	struct stat fileStats;
	int fileDescriptor = open(fileName, 0);

	if (fileDescriptor < 0) {
		fprintf(stderr, "file %s open failed\n", fileName);
		return NULL;
	}
	fstat(fileDescriptor, &fileStats);
	result = (char *) malloc(fileStats.st_size+1); // + null terminator

	if (!read(fileDescriptor, result, fileStats.st_size)) {
		fprintf(stderr, "file read error\n");
	}
	result[fileStats.st_size] = '\0';
	close(fileDescriptor);
	return result;
}
/*char *AllData = NULL;
    int FD = open(FileName, O_RDONLY);
    if (FD < 0) {
        fprintf(stderr, "file %s open failed\n", FileName);
        return;
    }
    struct stat StatBuf;
    fstat(FD, &StatBuf);
    AllData = (char *) malloc(StatBuf.st_size+1); // + null terminator

    if (!read(FD, AllData, StatBuf.st_size)) {
        fprintf(stderr, "file read error\n");
    }
    AllData[StatBuf.st_size] = '\0';
    close(FD);
    */
