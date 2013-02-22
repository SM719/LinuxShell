/*
 ============================================================================
 Name        : eece315_project1.c
 Author      : L2C - C4
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
int putenv(char* envString);//Removes compiler warning as fn doesn't have proper prototype

char** getPath(int* pathArrSize);
bool findPath(char** pathArr, int* pathArrSize, char* path);
bool addPath(char** pathArr, int* pathArrSize, char* path);
bool removePath(char** pathArr, int* pathArrSize, char* path);
bool setPath(char** pathArr, int pathArrSize);

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
	char* newPath="hello";
	//addPath(pathArr, pathArrSize, newPath);Killing it
	setPath(pathArr,*pathArrSize);
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
char** getPath(int* pathArrSize){
    char** result;
    char* currentPath;
    char* delimiter=malloc(sizeof(char));
    *delimiter=':';
    int index_currentPath=0;

    //Move first part of Path into array, if it doesn't exist then return.
    char* inputPath=getenv ("PATH");
    if(inputPath==NULL){
    	return NULL;
    }
    currentPath=strtok(getenv ("PATH"), delimiter);
    if(currentPath!=NULL){
    	result=(char **) malloc(sizeof (char *));
    	result[index_currentPath]=currentPath;
    }else{
    	*pathArrSize=0;
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
    *pathArrSize=index_currentPath+1;
	return result;
}

/*
 * Results: path will be added to pathArr if not already their
 * Returns: True if path exists in pathArr at fn return, false otherwise
 */
bool addPath(char** pathArr, int* pathArrSize, char* path){
	if(path==NULL){
		return false;
	}
	if(findPath(pathArr, pathArrSize, path)){
		return true;
	}
	pathArr=(char **) realloc(pathArr, sizeof(char *)*((*pathArrSize)+1));
	pathArr[*pathArrSize]=path;
	*pathArrSize+=1;
	return true;
}
/*
 * Results: path will be removed from pathArr if it exists
 * Returns: True if path doesn't exist in pathArr at fn return, false otherwise
 */
bool removePath(char** pathArr, int* pathArrSize, char* path){
	return true;
}
/*
 * Returns: True if path found in pathArr, false otherwise
 */
bool findPath(char** pathArr, int* pathArrSize, char* path){
	return false;
}
/*
 * Results: pathArr will be used to set the "PATH" environment variable
 * Returns: True if successful, false otherwise
 */
bool setPath(char** pathArr, int pathArrSize){
	int pathSize=0;

	//Calculate the characters from paths
	for(int i=0;i<pathArrSize; i++){
		pathSize+=strlen(pathArr[i]);//Fucking UP......
	}
	//Calculate the characters from delimiters +5 for "PATH="
	pathSize+=(pathArrSize-1)+5;

	//Allocate memory for resulting path
	char* systemPath=malloc(pathSize);

	//Build resulting path to be written
	systemPath[0]=0;//Clear first byte so strcat always sees as empty string
	strcat(systemPath,"PATH=");
	for(int i=0;i<pathArrSize; i++){
		strcat(systemPath,pathArr[i]);
		//If not the last argument then append delimiter
		if(i!=pathArrSize-1){
			strcat(systemPath,":");
		}
	}
	//attempt to write the path to the system and return the results
	if(putenv(systemPath)==0){
		return true;
	}else
		return false;

}
