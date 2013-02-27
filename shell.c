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
#define WHITESPACE (char[]){' ','~',',','\t','\n'}

//Global Variables
int backgroundFlag;


//Struct to pass args for our shell commands
struct command_t{ char *name;
    int argc;
    char *argv[MAX_ARGS];
};
//Struct hold local environment variables
struct environmentVariable_t{
	char *name;
	char *value;
};


//Function Declarations
//Library Functions without Prototypes -- Removes compiler warnings
int setenv(char* name, char* envString, int overwrite);
int gethostname (char *__name, size_t);
int kill (__pid_t __pid, int __sig);
char *strsep (char **__restrict __stringp,__const char *__restrict __delim);
//User Data Entry
void readUserCommand(char *userCommandEntered);
void printPromptMessage(void);
int parseCommandEntered(char *, struct command_t *,struct environmentVariable_t**, int*);
char* getAbsolutePathForCommand(char *pathArr[], char *commandName, int getNumberOfFolders);
//Path Functions
char** getPath(int* pathArrSize);
int findPath(char** pathArr, int* pathArrSize, char* path);
char** addPath(char** pathArr, int* pathArrSize, char* path);
bool removePath(char** pathArr, int* pathArrSize, char* path);
bool exportPath(char** pathArr, int pathArrSize);
//Local Environment Variable Functions
struct environmentVariable_t* createEnvVar(char* rawEnvVariable);
struct environmentVariable_t** addToLocalEnvVars(struct environmentVariable_t** localVariables, int* localVariablesCount, struct environmentVariable_t* envVar);
int findLocalEnvVar(struct environmentVariable_t** localVariables, int* localVariablesCount, char* envVarName);



//MAIN FUNCTION
int main(int argc, char *argv[])
{
    int pid,  status, getNumberOfFolders;
    char userCommandEntered[80];
    char  *absolutePath;
    struct command_t command;
    backgroundFlag = 0;
    //Get Path
    int* pathArrSize=malloc(sizeof(int));
    char** pathArr=getPath(pathArrSize);
    
    //Create Local Environment Variables
    int* localEnvVariablesCount=malloc(sizeof(int));
    *localEnvVariablesCount=0;
    struct environmentVariable_t** localEnvVariables=NULL;
    
    printPromptMessage();
    
    //GET USER INPUT
    readUserCommand(userCommandEntered);
    
    //Get the number of of folders we need to search
	getNumberOfFolders = pathArrSize[0];
    
    //see if the user wants to exit the shell
    while (strcmp (userCommandEntered, "exit\n") != 0)
    {
        parseCommandEntered(userCommandEntered, &command, localEnvVariables,localEnvVariablesCount);
        //CHECK FOR SHELL COMMANDS
        //change directory
        if (strcmp(command.name,"cd")==0)
        {
            int temp = chdir(command.argv[1]);
            if(temp == -1) printf("No such file or directory\n");
        }//export
        else if (strcmp(command.name,"export")==0){
        	if(command.argv[1]!=NULL && command.argv[2]!=NULL){
        		//Check to see if we are creating and exporting an existing variable or creating a new one
        		if(strchr(command.argv[1],'=')==NULL){
        			//Values already separated by parseCommand into two arguments
        			setenv(command.argv[1],command.argv[2],1);
        		}
        		else{
        			//Create Local EnvVar then export it
        			struct environmentVariable_t* envVar=createEnvVar(command.argv[1]);
        			localEnvVariables=addToLocalEnvVars(localEnvVariables,localEnvVariablesCount, envVar);
        			setenv(envVar->name,envVar->value,1);
        		}
        	}
        }
        else{
            
            absolutePath = getAbsolutePathForCommand(pathArr, command.argv[0], getNumberOfFolders);
            
            //IF PATH RETURNED IS A VALID PATH THEN EXECUTE THE COMMAND, ELSE PRINT 'NOT FOUND'
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
                if(backgroundFlag == 0)
                {
                    
                    wait(&status);
                }
                else {backgroundFlag = 0;}
                
                
                
            }//Check to see if they were creating a local environment variable
            else if(command.argc != 0 && strchr(command.argv[0],'=')!=NULL){
            	localEnvVariables=addToLocalEnvVars(localEnvVariables,localEnvVariablesCount,createEnvVar(command.argv[0]));
            }
            else{
            	printf("%s: command not found\n", command.argv[0]);

            }
            
        }
        //See if the user wants to continue executing commands
        printPromptMessage();
        readUserCommand(userCommandEntered);
	}
	return 0;
}

struct environmentVariable_t* createEnvVar(char* rawEnvVariable){
	struct environmentVariable_t* result=malloc(sizeof(struct environmentVariable_t));

	char* delimiter=malloc(sizeof(char));
	*delimiter='=';

	//Assign Name
	char* temp;
	temp=strtok(rawEnvVariable, delimiter);
	result->name=malloc(strlen(temp));
	strcpy(result->name,temp);
	//Assign Value
	*delimiter='\n';
	temp=strtok(NULL, delimiter);
	result->value=malloc(strlen(temp));
	strcpy(result->value,temp);

	free(delimiter);
	return result;
}
/*
* Results: adds envVar to the localVariables if it doesn't exist or overwrites the old value if it does
* Returns: the index where the envVar was written
*/
struct environmentVariable_t** addToLocalEnvVars(struct environmentVariable_t** localVariables, int* localVariablesCount, struct environmentVariable_t* envVar){
	if(envVar==NULL){
		exit(EXIT_FAILURE);//Null envVar
	}

	int index;
	//Check if local path exists
	if(localVariables==NULL || (index=findLocalEnvVar(localVariables,localVariablesCount,envVar->name))==-1){
		if((localVariables=(struct environmentVariable_t**) realloc(localVariables, sizeof(struct environmentVariable_t*)*((*localVariablesCount)+1)))==NULL){
			exit(EXIT_FAILURE);//Alloc Failed
		}
		index=*localVariablesCount;
		*localVariablesCount+=1;
	}
	//Assigns the envVar to localVariables either adding it or overwriting the previous value
	localVariables[index]=envVar;
	return localVariables;
	//return index;
}
/*
* Returns: index of envVar if found in localVariables, -1 otherwise
*/
int findLocalEnvVar(struct environmentVariable_t** localVariables, int* localVariablesCount, char* envVarName){
	int index;
	for(index=0;index<*localVariablesCount; index++){
		if(strcmp(localVariables[index]->name, envVarName)==0)
			return index;
	}
	return -1;
}
int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct,struct environmentVariable_t** localEnvVariables, int* localEnvVariablesCount) {
    
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
    		//Check if we are running an export command
    		if(strcmp(commandStruct->argv[0],"export")==0){
				int localEnvVarIndex;
    			//Check Local environment variable
    			if((localEnvVarIndex=findLocalEnvVar(localEnvVariables,localEnvVariablesCount,commandStruct->argv[argc]+1))!=-1){
    				//If Found Ready Arguments for the export command
    				commandStruct->argv[argc]=localEnvVariables[localEnvVarIndex]->name;
    				commandStruct->argv[++argc] = (char *) malloc(16);
    				commandStruct->argv[argc]=localEnvVariables[localEnvVarIndex]->value;
    				break;
    			}

    		}//Substitute the Value for the Variable normally
    		else{
				char* EnvVarValue;
				int localEnvVarIndex;
				//Check Local environment variable
				if((localEnvVarIndex=findLocalEnvVar(localEnvVariables,localEnvVariablesCount,commandStruct->argv[argc]+1))!=-1){
					commandStruct->argv[argc]=localEnvVariables[localEnvVarIndex]->value;
				}//Checks Global Environment Variable
				else if((EnvVarValue=getenv(commandStruct->argv[argc]+1))!=NULL){
					commandStruct->argv[argc] =EnvVarValue;//Assign value of variable to arg position
				}
    		}
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
	char *getCD;
	getCD = (char*) malloc(100);
    
    
    gethostname(compName, 80); //Get the name of the computer
	getCD=getenv ("PWD");
	
    
	//Print shell prompt message
    printf("GroupC4Shell- %s@%s:%s: ", getlogin(), compName, getCD );
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
    char* temptInputPath=getenv ("PATH");
	inputPath = (char *) malloc (strlen(temptInputPath));
	strcpy(inputPath,temptInputPath);
    
    
    
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
    free(delimiter);
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
 * Notes: exportPath doesn't persist between sessions
 * http://www.cplusplus.com/forum/beginner/48228/
 */
bool exportPath(char** pathArr, int pathArrSize){
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
		free(systemPath);
		return true;
	}else
		free(systemPath);
		return false;
}
