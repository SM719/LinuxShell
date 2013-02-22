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
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static const char _env_FPath[]= "/etc/environment";//UBUNTU 12.10 Path

char* getFile(const char* fileName);
char** getPath(int* size);
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
	int* pathArrSize=malloc(sizeof(int));
	char** pathArr=getPath(pathArrSize);
	char* input=(char*) malloc(255);


//Testing
	if(pathArr==NULL){
		puts("getEnv failed");
	}else{
		for(int i=0;i<*pathArrSize;i++){
			puts(pathArr[i]);
		}
	}

//Code
		scanf("%s", input);
		if(input!=NULL){
			puts(input);
		}

		getchar();
		getchar();

//Clean-up
		//free pathARR and all contained paths?
	return EXIT_SUCCESS;
}


/*
 * Results: The size of the path array will be set in size, the array of separated paths will be returned, else null
 */
char** getPath(int* size){
    char** result;
    char* currentPath;
    char* delimiter=malloc(sizeof(char));
    *delimiter=':';
    int index_currentPath=0;

    //Move first part of Path into array, if it doesn't exist then return.
    currentPath=strtok(getenv ("PATH"), delimiter);
    if(currentPath!=NULL){
    	result=(char **) malloc(sizeof (char *));
    	result[index_currentPath]=currentPath;
    }else{
    	*size=0;
    	return NULL;
    }

    //Move every other Path into array
    currentPath=strtok(NULL, delimiter);
    while(currentPath != NULL){
    	index_currentPath++;
    	result=(char **) realloc(result, sizeof (char *)*(index_currentPath+1));//increase pointer array to hold results
    	result[index_currentPath]=currentPath;
        currentPath=strtok(NULL, delimiter);
    }

    //Set the size from the index and return the results
    *size=index_currentPath+1;
	return result;
}
/*
 * Assumes one entry per line
 */
/*
char* getEnv(int* size){
	char* string=getFile(_env_FPath);
	int variablesCount=0;
	//strlen(string)
	int nextVariableIndex[strlen(string)];//Make this as big as possibly necessary
	int temp=sizeof(nextVariableIndex);
	nextVariableIndex[0]=4;

	//Find number of env variables
	for(int index=0; index<strlen(string);index++){
		if(string[index]=='\n'){
			nextVariableIndex[variablesCount]=index+1;//Add the start of the next string
			variablesCount++;
		}
	}

	char* result[variablesCount];
	//Move Variables into Array
	for(int index=0; index<variablesCount;index++){
		//TODO
		if(index!=0){
			result[index]=malloc((nextVariableIndex[index]-2)-nextVariableIndex[index-1]);
			strncpy(result[index], string+nextVariableIndex[index-1], sizeof(*result[index]));
		}else{
			result[index]=malloc((nextVariableIndex[index]-2)-nextVariableIndex[index-1]);
			strncpy(result[index], string, sizeof(*result[index]));
		}
	}
	*size=variablesCount;
	return *result;
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
char* getFile(const char* fileName){
	char *result = NULL;
	struct stat fileStats;
	int fileDescriptor = open(fileName, O_RDONLY);

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
