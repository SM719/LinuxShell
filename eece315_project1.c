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
int setenv(char* name, char* envString, int overwrite);//Removes compiler warning as fn doesn't have proper prototype

void exportEnv(char* envString);
char** getPath(int* pathArrSize);
int findPath(char** pathArr, int* pathArrSize, char* path);
char** addPath(char** pathArr, int* pathArrSize, char* path);
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
	char* envTest="lol=hi";
	exportEnv(envTest);//Broken
	if(pathArr==NULL){
		puts("getEnv failed");
	}else{
		for(int i=0;i<*pathArrSize;i++){
			puts(pathArr[i]);
		}
	}
	char* testPath="/hi";
	pathArr=addPath(pathArr, pathArrSize, testPath);
	bool setSuccess=setPath(pathArr,*pathArrSize);
	pathArr=getPath(pathArrSize);
	if(pathArr==NULL){
			puts("getEnv failed");
		}else{
			for(int i=0;i<*pathArrSize;i++){
				puts(pathArr[i]);
			}
		}
	testPath="/hello";
	setSuccess=removePath(pathArr, pathArrSize, testPath);
	if(pathArr==NULL){
		puts("getEnv failed");
	}
	else{
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
    	exit(EXIT_FAILURE);//GetEnv  failed
    }
    currentPath=strtok(inputPath, delimiter);
    if(currentPath!=NULL){
    	if((result=(char **) malloc(sizeof (char *)))==NULL){
    		exit(EXIT_FAILURE);//Alloc Failed
    	}
    	result[index_currentPath]=currentPath;
    }else{
    	*pathArrSize=0;
    	return NULL;
    }

    //Move every other Path into array
    currentPath=strtok(NULL, delimiter);
    while(currentPath != NULL){
    	index_currentPath++;
    	//increase pointer array to hold results
    	if((result=(char **) realloc(result, sizeof (char *)*(index_currentPath+1)))==NULL){
    		exit(EXIT_FAILURE);//Alloc Failed
    	}
    	result[index_currentPath]=currentPath;
        currentPath=strtok(NULL, delimiter);
    }

    //Set the size from the index and return the results
    *pathArrSize=index_currentPath+1;
	return result;
}

/*
 * Results: path will be added to pathArr if not already there
 * Returns: pathArr
 * Notes: modifying pathArr without returning&assigning it messes up memory somehow?-wTf
 */
char** addPath(char** pathArr, int* pathArrSize, char* path){
	if(path==NULL){
		exit(EXIT_FAILURE);//Null path
	}
	if((findPath(pathArr, pathArrSize, path)!=-1)){
		return pathArr;
	}
	if((pathArr=(char **) realloc(pathArr, sizeof(char *)*((*pathArrSize)+1)))==NULL){
		exit(EXIT_FAILURE);//Alloc Failed
	}
	pathArr[*pathArrSize]=path;
	*pathArrSize+=1;
	return pathArr;
}
/*
 * Results: path will be removed from pathArr if it exists
 * Returns: True if path existed and removed, false if path didn't exist
 */
bool removePath(char** pathArr, int* pathArrSize, char* path){
	if(path==NULL){
		exit(EXIT_FAILURE);//Null path
	}
	int index=findPath(pathArr, pathArrSize, path);
	if(index==-1){
		return false;
	}
	//Remove Path and adjust pathArr
	for(;index<(*pathArrSize)-1;index++){
		pathArr[index]=pathArr[index+1];
	}
	//Decrease the array size
	*pathArrSize-=1;
	//Free extra memory
	if((pathArr=(char **) realloc(pathArr, sizeof(char *)*(*pathArrSize)))==NULL){
		exit(EXIT_FAILURE);//Alloc Failed
	}
	return true;
}
/*
 * Returns: index of path if found in pathArr, -1 otherwise
 */
int findPath(char** pathArr, int* pathArrSize, char* path){
	for(int i=0;i<*pathArrSize; i++){
		if(strcmp(pathArr[i],path)==0)
			return i;
	}
	return -1;
}
/*
 * Results: pathArr will be used to set the "PATH" environment variable
 * Returns: True if successful, false otherwise
 * Notes: setPath doesn't persist between sessions
 * http://www.cplusplus.com/forum/beginner/48228/
 */
bool setPath(char** pathArr, int pathArrSize){
	int pathSize=0;

	//Calculate the characters from paths
	for(int i=0;i<pathArrSize; i++){
		pathSize+=strlen(pathArr[i]);
	}
	//Calculate the characters from delimiters +5 for "PATH="
	pathSize+=(pathArrSize-1);//+5;

	//Allocate memory for resulting path
	char* systemPath=malloc(pathSize);

	//Build resulting path to be written
	systemPath[0]=0;//Clear first byte so strcat always sees as empty string
	//strcat(systemPath,"PATH=");
	for(int i=0;i<pathArrSize; i++){
		strcat(systemPath,pathArr[i]);
		//If not the last argument then append delimiter
		if(i!=pathArrSize-1){
			strcat(systemPath,":");
		}
	}
	//Attempt to write the path to the system and return the results
	if((setenv("PATH",systemPath,1))==0){
		return true;
	}else
		return false;
}
/*
 * Returns: Environment variable or null if not found
 * Note: Check if it is null before displaying
 */
char* getEnv(char* envName){
	return getenv(envName);
}
/*
 * Returns: Environment variable or null if not found
 * Note: Check if it is null before displaying
 */
void exportEnv(char* envString){
	char* name=malloc(sizeof(char*));
	char* value=malloc(sizeof(char*));
	char* delimeters=malloc(sizeof(char));
	*delimeters='=';
	//value=
	//strtok(envString,delimeters);
	//name=strtok(NULL, delimeters);
	 char* delimiter=malloc(sizeof(char));
	    *delimiter='=';
	    if(envString!=NULL){
	    	name=strtok(envString, delimiter);//Broken
	    }
	int x;
}
