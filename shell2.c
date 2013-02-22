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

#define MAX_ARGS 64

#define MAX_ARG_LEN 16

#define MAX_LINE_LEN 80

#define WHITESPACE " ~,\t\n"

//Struct to pass args for our shell commands
struct command_t { char *name;
        int argc;
        char *argv[];
};
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
	char* checkCommandPath(char **d2array, char *cmd, int count);



int main(void) {
//Simple Test to ensure program started
        puts("Shell Started...");

//Variables
        int* pathArrSize=malloc(sizeof(int));
        char** pathArr=getPath(pathArrSize);
        int pid, numChildren, status, count;
        struct command_t command; // Shell initialization
        char userCommandEntered[80];
	char *absolutePath;
	int temp = 0;

//Code
        while(strcmp (userCommandEntered, "exit\n") != 0) {
                printPromptMessage();
                readUserCommand(userCommandEntered);
		temp =  parseCommandEntered(userCommandEntered, &command);
		temp = pathArrSize[0];
		absolutePath = checkCommandPath(pathArr, command.argv[0], temp);
                //Execute commands
                //Either traverse file tree or run programs
                if((pid = fork()) == 0)
		{
			execv(absolutePath, command.argv);

        	}

//Clean-up
               } //free pathARR and all contained paths?
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



//FUNCTION TO FIND THE ABSOLUTE PATH OF COMMAND ENTERED BY THE USER
char* checkCommandPath(char **d2array, char *cmd, int count){

    //FUNCTION VARIABLES   
    int i = 1;
    struct stat st;
    char *c3, *cmd2, *slash = {"/"};

    //COPY COMMAND ENTERED BY USER TO 'cmd2'
    cmd2 = (char *)malloc(strlen(cmd)+1);
    strcpy(cmd2,cmd);

    //IF COMMAND STARTS WITH '/' THEN RETURN THE COMMAND ITSELF AS IT IS ALREADY AN ABSOLUTE PATH
    if (cmd[0] == '/')
    {
        cmd2 = strsep(&cmd2, " ");
        if (stat(cmd2,&st)== 0)
			return(cmd2);
        else
            return("not found file");       
    }

    //ITERATE THRU EVERY FOLDER IN PATH TO SEARCH IF THE FILE EXISTS
    for (i =1; i <= count ; i++)
    {
		c3 = (char *)malloc(strlen(cmd) + strlen(d2array[i]) + 2);
		strcpy(c3,d2array[i]);
		strcat(c3,slash);
        strcpy(cmd2,cmd);
        cmd2 = strsep(&cmd2, " ");
        cmd2 = strsep(&cmd2, "\n");
        strcat(c3,cmd2);

        //CALL SYSTEM FUNCTION TO CHECK IF THE FILE EXISTS. IF IT DOES THEN BREAK FROM LOOP AND RETURN THE FOLDER THAT THE EXECUTABLE WAS FOUND AT
        if(stat(c3,&st)== 0)
		{
			return (c3);
		}
	}
   
    //IF COMMAND IS NOT FOUND IN ANY OF THE FOLDERS IN PATH THEN RETURN 'not found'
	return "not found";
}    

int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct) {

        int argc=0;

        char **clPtr;


        /* Initialization */

        clPtr = &userCommandEntered; /* userCommandEntered is the command line */
        commandStruct->argv[0] = (char *) malloc(MAX_ARG_LEN);


        while((commandStruct->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL)

        {

                commandStruct->argv[++argc] = (char *) malloc(MAX_ARG_LEN);

        }



        /* Set the command name and argc */

        commandStruct->argc = (argc - 1);

        commandStruct->name = (char *) malloc(sizeof(commandStruct->argv[0]));

        strcpy(commandStruct->name, commandStruct->argv[0]);
        
        return 1;
}
