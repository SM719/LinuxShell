/*
 ============================================================================
 Name        : eece315_project1.c
 Author      : L2C - Group C4
 Version     :
 Copyright   :
 Description : Project 1 - Linux Shell
 Notes		 : Compile with C99 Standard
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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

//Global Varriables
int backgroundFlag;


//Struct to pass args for our shell commands
struct command_t{ char *name;
    int argc;
    char *argv[MAX_ARGS];
};


//Function Declarations
//Library Functions without Prototypes -- Removes compiler warnings
//int setenv(char* name, char* envString, int overwrite);
int gethostname (char *__name, size_t);
int kill (__pid_t __pid, int __sig);
char *strsep (char **__restrict __stringp,__const char *__restrict __delim);
//User Data Entry
void readUserCommand(char *userCommandEntered);
char** getPath(int* pathArrSize);
void printPromptMessage(int commandCount);
int parseCommandEntered(char *, struct command_t *);
char* getAbsolutePathForCommand(char *pathArr[], char *commandName, int getNumberOfFolders);
//Path Functions
char** getPath(int* pathArrSize);
int findPath(char** pathArr, int* pathArrSize, char* path);
char** addPath(char** pathArr, int* pathArrSize, char* path);
bool removePath(char** pathArr, int* pathArrSize, char* path);
bool setPath(char** pathArr, int pathArrSize);


//MAIN FUNCTION
int main(int argc, char *argv[])
{
    int pid,  status, getNumberOfFolders;
    char userCommandEntered[80];
    char  *absolutePath;
    struct command_t command;
    backgroundFlag = 0;
	int commandCount = 0;
    //Get Path
    int* pathArrSize=malloc(sizeof(int));
    char** pathArr=getPath(pathArrSize);
    
    //Create Local Environment Variables
    int* localVariablesCount=malloc(sizeof(int));
    char** localVariables;
    
    printPromptMessage(commandCount);
    
    //GET USER INPUT
    readUserCommand(userCommandEntered);
    
    //Get the number of of folders we need to search
	getNumberOfFolders = pathArrSize[0];
    
    //see if the user wants to exit the shell
    while (strcmp (userCommandEntered, "exit\n") != 0)
    {
        
        parseCommandEntered(userCommandEntered, &command);
        //CHECK FOR SHELL COMMANDS
        //change directory
        if (strcmp(command.name,"cd")==0)
        {
            int temp = chdir(command.argv[1]);
            if(temp == -1) printf("No such file or directory\n");
        }//export
        else if (strcmp(command.name,"export")==0){
        	if(command.argv[1]!=NULL && command.argv[2]!=NULL){
        		//Parse Environment Variable parseEnv(char* input);
        		//setenv(command.argv[1],command.argv[2], 1);
        		//DEBUG PRINT getenv of that....
        	}
        }//create local variable
        else{
            
            absolutePath = getAbsolutePathForCommand(pathArr, command.argv[0], getNumberOfFolders);
            
            
            //Only execute the command if the path is valid otherwise print command not found
            if ((strcmp(absolutePath,"not found") != 0) )
            {
                
                //IF THE COMMAND ENTERED IS ONLY ONE ARGUMENT THEN DO NOT SET THE LAST ARGUMENT AS NULL
                if(command.argc != 0)
                {
                    command.argv[command.argc] = NULL;
                }
                
                
                //Excute the command in child process
                if((pid = fork()) == 0)
                {
                    
					execv(absolutePath, command.argv);
                }
                //If the run in background flag isn't set wait for child process to finish before continuing
                if(backgroundFlag == 0)
                {
                    
                    wait(&status);
                }
                else {backgroundFlag = 0;}
                
                
                
            }
            else if (strcmp(absolutePath,"not found") == 0)
            {
                printf("%s: command not found\n", command.argv[0]);
            }
            
        }
        //See if the user wants to continue executing commands
        commandCount++;
        printPromptMessage(commandCount);
        readUserCommand(userCommandEntered);
	}
	return 0;
}

int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct) {
    
    int argc=0;
    char **clPtr;
	int length;
	char *find;
	
    
    /* Initialization */
    clPtr = &userCommandEntered; /* userCommandEntered is the command line */
    commandStruct->argv[0] = (char *) malloc(16);
    
    while((commandStruct->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL)
    {
    	//Check for Environment Variable argument, process and store.
    	if(argc>0 && commandStruct->argv[argc][0]=='$'){
    		char* var=malloc(sizeof(char*));
    		//Checks Global Environment Variable
    		if((var=getenv(commandStruct->argv[argc]+1))!=NULL){
    			commandStruct->argv[argc] =var;//Assign value of variable to arg position
    		}
    		//TODO Check Local environment variable
    	}
        
        
		if(strcmp(commandStruct->argv[argc],"&") == 0 )
		{
			commandStruct->argv[argc] = " ";
			argc++;
			backgroundFlag = 1;
			break;
		}
        
		else
		{
            length = strlen(commandStruct->argv[argc]) - 1;
            find = commandStruct->argv[argc];
            
            if (find[length] == '&')
            {
				find = strsep(&find, "&");
				commandStruct->argv[argc] = find;
				argc = argc + 2;
				backgroundFlag = 1;
				break;
			}
		}
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
    
	//Make a copy of the command entered in commandName2
    commandName2 = (char *)malloc(strlen(commandName)+1);
    strcpy(commandName2,commandName);
    
    
	//Check if the command starts with an / if so return as its already an absolute path
    if (commandName[0] == '/')
    {
        commandName2 = strsep(&commandName2, " ");
        if (stat(commandName2,&st)== 0)
			return(commandName2);
        else
            return("not found file");
    }
    
    
	//Check every folder in the path to see if the file exits
    for (indexValue =0; indexValue < getNumberOfFolders ; indexValue++)
    {
		c3 = (char *)malloc(strlen(commandName) + strlen(pathArr[indexValue]) + 2);
		strcpy(c3,pathArr[indexValue]);
		strcat(c3,slash);
        strcpy(commandName2,commandName);
        commandName2 = strsep(&commandName2, " ");
        commandName2 = strsep(&commandName2, "\n");
        strcat(c3,commandName2);
        
        
        //Check if teh file exists using a sytem call and if so then return the folder that the executable was found in
        if(stat(c3,&st)== 0)
		{
			return (c3);
		}
	}
    
	//If the command is not found after search all the folders return the string not found
	return "not found";
}

void readUserCommand(char *userCommandEntered){
    
    fgets (userCommandEntered, 80, stdin);
}

//Returns nothing prints the shell prompt message
void printPromptMessage(int commandCount){
    
    char compName[80];
    
	char *getCD, *CDbuffer;
	getCD = (char*) malloc(100);
	CDbuffer = (char*) malloc(100);
	getCD = getcwd(CDbuffer, 99);  //Get the path of the current directory
    
    
    gethostname(compName, 80); //Get the name of the computer
    
    printf("Commands Entered %d - %s@%s:%s: ", commandCount, getlogin(), compName, getCD );
}

//PATH FUNCTIONS
////////////////
/*
 * Results: The size of the path array will be set in size, the array of separated paths will be returned, else null
 */
char** getPath(int* pathArrSize){
    char** result;
    char* currentPath;
    char* delimiter=malloc(sizeof(char));
    *delimiter=':';
    int index_currentPath=0;
	char * inputPath;
	
    
    //Move first part of Path into array, if it doesn't exist then return.
    char* inputPath2=getenv ("PATH");
	inputPath = (char *) malloc (strlen(inputPath2));
	strcpy(inputPath,inputPath2);
    
    
    
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
	
	int i;
	for(i=0;i<*pathArrSize; i++){
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
	int i;
	for(i=0;i<pathArrSize; i++){
		pathSize+=strlen(pathArr[i]);
	}
	//Calculate the characters from delimiters +5 for "PATH="
	pathSize+=(pathArrSize-1);//+5;
    
	//Allocate memory for resulting path
	char* systemPath=malloc(pathSize);
    
	//Build resulting path to be written
	systemPath[0]=0;//Clear first byte so strcat always sees as empty string
	//strcat(systemPath,"PATH=");
	for(i=0;i<pathArrSize; i++){
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