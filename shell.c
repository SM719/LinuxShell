/*
 ============================================================================
 Name        : eece315_project1.c
 Author      : L2C - Group C4
 Version     :
 Copyright   : 
 Description : Project 1 - Linux Shell
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

//CONSTANTS USED IN THE PROGRAM
#define MAX_ARGS 64
#define WHITESPACE " ~,\t\n"


//Struct to pass args for our shell commands
struct command_t{ char *name;
    int argc;
    char *argv[MAX_ARGS];
};


//Function Declartions
void readUserCommand(char *userCommandEntered);
char** getPath(int* pathArrSize);
void printPromptMessage(void);
int parseCommand(char *, struct command_t *);
char* getAbsolutePathForCommand(char *pathArr[], char *commandName, int getNumberOfFolders);



//MAIN FUNCTION
int main(int argc, char *argv[])
{
    int pid,  status, getNumberOfFolders;
    char userCommandEntered[80];
    char  *absolutePath;
    struct command_t command;
    int* pathArrSize=malloc(sizeof(int));
    char** pathArr=getPath(pathArrSize);

    printPromptMessage();

    //GET USER INPUT
    readUserCommand(userCommandEntered);

    //Get the number of of folders we need to search
	getNumberOfFolders = pathArrSize[0];

    //see if the user wants to exit the shell
    while (strcmp (userCommandEntered, "exit\n") != 0)
    {

        parseCommandEntered(userCommandEntered, &command);

        if (strcmp(command.name,"cd")==0)
        {
            int temp = chdir(command.argv[1]);
            if(temp == -1) printf("No such file or directory\n");
        }



        else{
            //parseCommandEntered(userCommandEntered, &command);


            absolutePath = getAbsolutePathForCommand(pathArr, command.argv[0], getNumberOfFolders);

            //IF PATH RETURNED IS A VALID PATH THEN EXECUTE THE COMMAND ELSE PRINT 'NOT FOUND'
            if ((strcmp(absolutePath,"not found") != 0) )
            {

                //IF THE COMMAND ENTERED IS ONLY ONE ARGUMENT THEN DO NOT SET THE LAST ARGUMENT AS NULL
                if(command.argc != 0)
                {
                    command.argv[command.argc] = NULL;
                }

                //EXECUTE THE COMMAND IN THE CHILD PROCESS.
                //IF FOR SOME REASON THE EXECV COMMANDS FAIL AND CHILD PROCESS IS NOT TERMINATED THEN 			KILL THE PROCESS MANUALLY.
                if((pid = fork()) == 0)
				{

					execv(absolutePath, command.argv);
					pid_t pidchild = getpid();
					kill(pidchild,SIGKILL);
                }

                wait(&status);


            }
            else if (strcmp(absolutePath,"not found") == 0)
            {
                printf("%s: command not found\n", command.argv[0]);
            }



        }
        //See if the user wants to continue executing commands
        printPromptMessage();
        readUserCommand(userCommandEntered);

	}

	return 0;
}

int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct) {

    int argc=0;

    char **clPtr;


    /* Initialization */

    clPtr = &userCommandEntered; /* userCommandEntered is the command line */
    commandStruct->argv[0] = (char *) malloc(16);


    while((commandStruct->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL)

    {

        commandStruct->argv[++argc] = (char *) malloc(16);

    }



    /* Set the command name and argc */

    commandStruct->argc = (argc - 1);

    commandStruct->name = (char *) malloc(sizeof(commandStruct->argv[0]));

    strcpy(commandStruct->name, commandStruct->argv[0]);

    return 1;
}

//Function returns absolute path of the cmd typed by user
char* getAbsolutePathForCommand(char *pathArr[], char *commandName, int getNumberOfFolders){

    int indexValue = 1;
    struct stat st;
    char *c3, *commandName2, *slash = {"/"};

    //COPY COMMAND ENTERED BY USER TO 'commandName2'
    commandName2 = (char *)malloc(strlen(commandName)+1);
    strcpy(commandName2,commandName);

    //IF COMMAND STARTS WITH '/' THEN RETURN THE COMMAND ITSELF AS IT IS ALREADY AN ABSOLUTE PATH
    if (commandName[0] == '/')
    {
        commandName2 = strsep(&commandName2, " ");
        if (stat(commandName2,&st)== 0)
			return(commandName2);
        else
            return("not found file");
    }

    //ITERATE THRU EVERY FOLDER IN PATH TO SEARCH IF THE FILE EXISTS
    for (indexValue =0; indexValue < getNumberOfFolders ; indexValue++)
    {
		c3 = (char *)malloc(strlen(commandName) + strlen(pathArr[indexValue]) + 2);
		strcpy(c3,pathArr[indexValue]);
		strcat(c3,slash);
        strcpy(commandName2,commandName);
        commandName2 = strsep(&commandName2, " ");
        commandName2 = strsep(&commandName2, "\n");
        strcat(c3,commandName2);

        //CALL SYSTEM FUNCTION TO CHECK IF THE FILE EXISTS. IF IT DOES THEN BREAK FROM LOOP AND RETURN THE FOLDER THAT THE EXECUTABLE WAS FOUND AT
        if(stat(c3,&st)== 0)
		{
			return (c3);
		}
	}

    //IF COMMAND IS NOT FOUND IN ANY OF THE FOLDERS IN PATH THEN RETURN 'not found'
	return "not found";
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
