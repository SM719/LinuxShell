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

//Function Declarations
        //User Input
        void printPromptMessage(void);
        void readUserCommand(char *userCommandEntered);
        int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct);
        void checkIfUserChangesDirectory(char *userCommandEntered);
        //Path
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
        int pid, numChildren, status, count;
        struct command_t command; // Shell initialization
        char userCommandEntered[80];

//Code
        while(strcmp (userCommandEntered, "exit\n") != 0) {
                printPromptMessage();
                readUserCommand(userCommandEntered);
                //Execute commands
                //Either traverse file tree or run programs

        }

//Clean-up
                //free pathARR and all contained paths?
        return EXIT_SUCCESS;
}


/*
 * Results: The size of the path array will be set in size
 * Returns: The array of separated paths will be returned, else null
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
void readUserCommand(char *userCommandEntered){

        fgets (userCommandEntered, 80, stdin);
}

void printPromptMessage(void){

        char compName[80];

        gethostname(compName, 80); //Get the name of the computer


        //Print shell prompt message
        printf("GroupC4Shell - %s@%s: ", getlogin(), compName );
}