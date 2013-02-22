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
char* checkCommandPath(char *pathArr[], char *cmd, int count);



//MAIN FUNCTION
int main(int argc, char *argv[])
{
    int pid,  status, count;
    char userCommandEntered[80];
    char  *absolutePath;
    struct command_t command;
    int* pathArrSize=malloc(sizeof(int));
    char** pathArr=getPath(pathArrSize);
   
    printPromptMessage();

    //GET USER INPUT
    readUserCommand(userCommandEntered);
   
    //Get the number of of folders we need to search
	count = pathArrSize[0];

    //see if the user wants to exit the shell
    while (strcmp (userCommandEntered, "exit\n") != 0)
    {
         
            parseCommandEntered(userCommandEntered, &command);
           
         
            absolutePath = checkCommandPath(pathArr, command.argv[0], count);
           
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


//FUNCTION TO FIND THE ABSOLUTE PATH OF COMMAND ENTERED BY THE USER
char* checkCommandPath(char *pathArr[], char *cmd, int count){

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
		c3 = (char *)malloc(strlen(cmd) + strlen(pathArr[i]) + 2);
		strcpy(c3,pathArr[i]);
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

void readUserCommand(char *userCommandEntered){

        fgets (userCommandEntered, 80, stdin);
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

void printPromptMessage(void){
        
        char compName[80];      

        gethostname(compName, 80); //Get the name of the computer


        //Print shell prompt message
        printf("GroupC4Shell - %s@%s: ", getlogin(), compName ); 
}
