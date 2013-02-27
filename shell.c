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
int unsetenv(char*);
int setenv(char* name, char* envString, int overwrite);
int gethostname (char *__name, size_t);
char *strsep (char **__restrict __stringp,__const char *__restrict __delim);
//User Data Entry
void readUserCommand(char *userCommandEntered);
int parseCommandEntered(char *, struct command_t *,struct environmentVariable_t**, int*,char**,int*);
void printPromptMessage(int commandCount);
char* getAbsolutePathForCommand(char *pathArr[], char *commandName, int getNumberOfFolders);
//Path Functions
char** getPath(int* pathArrSize);
int findPath(char** pathArr, int* pathArrSize, char* path);
char** addPath(char** pathArr, int* pathArrSize, char* path);
bool removePath(char** pathArr, int* pathArrSize, char* path);
char* buildExportPath(char** pathArr, int pathArrSize);
//Local Environment Variable Functions
struct environmentVariable_t* createEnvVar(char* rawEnvVariable);
struct environmentVariable_t** addToLocalEnvVars(struct environmentVariable_t** localVariables, int* localVariablesCount, struct environmentVariable_t* envVar);
int findLocalEnvVar(struct environmentVariable_t** localVariables, int* localVariablesCount, char* envVarName);
char *get_current_dir_name(void);
bool removeLocalEnvVar(struct environmentVariable_t** localVariables, int* localVariablesCount, char* envVarName);
//Help
void printHelp();


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
    int* pathArrCount=malloc(sizeof(int));
    char** pathArr=getPath(pathArrCount);
    
    //Create Local Environment Variables
    int* localEnvVariablesCount=malloc(sizeof(int));
    *localEnvVariablesCount=0;
    struct environmentVariable_t** localEnvVariables=NULL;
    
    printPromptMessage(commandCount);
    
    //GET USER INPUT
    readUserCommand(userCommandEntered);
    
    //Get the number of of folders we need to search
	getNumberOfFolders = pathArrCount[0];
    
    //see if the user wants to exit the shell
    while (strcmp (userCommandEntered, "exit\n") != 0)
    {


        parseCommandEntered(userCommandEntered, &command, localEnvVariables,localEnvVariablesCount,pathArr,pathArrCount);
        //CHECK FOR SHELL COMMANDS
        //change directory
        if (strcmp(command.name,"cd")==0)
        {
            int temp = chdir(command.argv[1]);
            if(temp == -1) printf("No such file or directory\n");

        }//addPath

        else if (strcmp(command.name,"addPath")==0){
        	if(strlen(command.argv[1])!=0){
        		pathArr=addPath(pathArr,pathArrCount,command.argv[1]);
        	}


		}//rmPath
        else if (strcmp(command.name,"rmPath")==0){
			removePath(pathArr,pathArrCount, command.argv[1]);
		}//getPath
        else if (strcmp(command.name,"getPath")==0){
			pathArr=getPath(pathArrCount);
		}//rmEnv
        else if (strcmp(command.name,"rmEnv")==0){
        	unsetenv(command.argv[1]);
		}//rmLocalEnv
        else if (strcmp(command.name,"rmLocalEnv")==0){
        	removeLocalEnvVar(localEnvVariables,localEnvVariablesCount,command.argv[1]);
		}//--help
        else if (strcmp(command.name,"--help")==0){
			printHelp();

		}
        //export
        else if (strcmp(command.name,"export")==0){
        	if(command.argv[1]!=NULL && command.argv[2]!=NULL){
        		//Check to see if we are creating and exporting an existing variable or creating a new one
        		if(strchr(command.argv[1],'=')==NULL){
        			//Existing Local EnvVar
        			//Values already separated by parseCommand into two arguments
        			setenv(command.argv[1],command.argv[2],1);
        		}
        		else{
        			//Create Local EnvVar then export it
        			struct environmentVariable_t* envVar=createEnvVar(command.argv[1]);
        			if(envVar!=NULL){
						localEnvVariables=addToLocalEnvVars(localEnvVariables,localEnvVariablesCount, envVar);
						setenv(envVar->name,envVar->value,1);
        			}
        		}
        	}
        }
        else{
            
            absolutePath = getAbsolutePathForCommand(pathArr, command.argv[0], getNumberOfFolders);
            
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
                	//Fetch the environment variable this process should have
                	pathArr=getPath(pathArrCount);
                	//Clear the local environment variables
                	localEnvVariablesCount=0;
                	localEnvVariables=NULL;
					execv(absolutePath, command.argv);
					return 0;
                }
                //If the run in background flag isn't set wait for child process to finish before continuing
                if(backgroundFlag == 0)
                {
                    waitpid(pid,&status,0);
                }
                else {backgroundFlag = 0;}
                
                
                
            }//Check to see if they were creating a local environment variable
            else if(command.argc != 0 && strchr(command.argv[0],'=')!=NULL){
            	struct environmentVariable_t* envVar=createEnvVar(command.argv[0]);
            	if(envVar!=NULL){
            		localEnvVariables=addToLocalEnvVars(localEnvVariables,localEnvVariablesCount,envVar);
            	}
            }
            else{
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
/*
 * Prints the help contents of the project
 */
void printHelp(){
	printf("LAB L2C - Group C4's Shell\n\n");
	printf("\t-varName=varValue to create or modify local environment variables\n");
	printf("\t-echo $varName to show local or session environment variables\n");
	printf("\t-export $varName or export varName=varValue to set session environment variables\n");
	printf("\t-rmEnv environmentVariableName to remove session environment variable\n");
	printf("\t-rmLocalEnv environmentVariableName to remove local environment variable\n");

	printf("\n\n To easily modify the path and add paths to the local session, use the following path builder: \n");
	printf("\t *If a local environment variable is created called \"PATH\", that will be used in export instead of this builder: \n");
	printf("\t-addPath path to add to the local path\n");
	printf("\t-rmPath path to remove from the local path\n");
	printf("\t-getPath sets the local path to the session environment path\n");
	printf("\t-export $PATH to build the session path and export it to the session environment variables if a local env Var doesn't exist\n");
	printf("\n");
}
/*
 * Returns a envVar struct or Null if the input was bad
 */
struct environmentVariable_t* createEnvVar(char* rawEnvVariable){
	struct environmentVariable_t* result=malloc(sizeof(struct environmentVariable_t));
    
	char* delimiter=malloc(sizeof(char));
	*delimiter='=';
    
	//Assign Name
	char* temp;
	temp=strtok(rawEnvVariable, delimiter);
	if(temp==NULL){
		return NULL;
	}
	result->name=malloc(strlen(temp));
	strcpy(result->name,temp);

	//Assign Value
	*delimiter='\n';
	temp=strtok(NULL, delimiter);
	if(temp==NULL){
		return NULL;
	}
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

/*
 * Results: envVarName will be removed from localVariables if it exists
 * Returns: True if path existed and removed, false if path didn't exist
 */
bool removeLocalEnvVar(struct environmentVariable_t** localVariables, int* localVariablesCount, char* envVarName){
	if(envVarName==NULL){
		exit(EXIT_FAILURE);//Null path
	}
	int index=findLocalEnvVar(localVariables,localVariablesCount,envVarName);
	if(index==-1){
		return false;
	}
	//Remove Path and adjust pathArr
	for(;index<(*localVariablesCount)-1;index++){
		localVariables[index]=localVariables[index+1];
	}
	//Decrease the array size
	*localVariablesCount-=1;
	//Free extra memory
	if(*localVariablesCount==0){
		free(localVariables);
		localVariables=NULL;
	}else if((localVariables=(struct environmentVariable_t**) realloc(localVariables, sizeof(struct environmentVariable_t**)*(*localVariablesCount)))==NULL){
		exit(EXIT_FAILURE);//Alloc Failed
	}
	return true;
}

int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct,struct environmentVariable_t** localEnvVariables, int* localEnvVariablesCount, char** pathArr, int* pathArrCount) {
    
    int argc=0;
    char **clPtr;
	int length;
	char *find;
	char *result;
	
    
    /* Initialization */
    clPtr = &userCommandEntered; /* userCommandEntered is the command line */
    commandStruct->argv[0] = (char *) malloc(16);
	
    
	if (userCommandEntered[0] == 'c' && userCommandEntered [1] == 'd') {
        char* delimiter=malloc(sizeof(char));
        *delimiter=' ';
        
        //Assign Name
        char* temp;
        temp=strtok(userCommandEntered, delimiter);
        if(temp==NULL){
            return 1;
        }
        result =malloc(strlen(temp));
        strcpy(result,temp);
        commandStruct->argv[0] = result;
        
        //Parse path
        *delimiter='\n';
        temp=strtok(userCommandEntered+3, delimiter);
        
        if(temp==NULL){
            return 1;
        }
        result=malloc(strlen(temp));
        strcpy(result,temp);
        commandStruct->argv[1] = result;
        
        free(delimiter);
        
        commandStruct->argc = 2;
    	commandStruct->name = (char *) malloc(sizeof(commandStruct->argv[0]));
    	strcpy(commandStruct->name, commandStruct->argv[0]);
        return 1;
        
        
	}
    
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
    			}else if(strcmp(commandStruct->argv[argc]+1,"PATH")==0){
    				//Ready Arguments for the export command
    				commandStruct->argv[argc]=malloc(sizeof("PATH"));
    				commandStruct->argv[argc]="PATH";
    				commandStruct->argv[++argc] = (char *) malloc(16);
					commandStruct->argv[argc]=buildExportPath(pathArr,*pathArrCount);
					break;
    			}
    		}//Substitute the Value for the Variable normally
    		else{
				char* EnvVarValue;
				int localEnvVarIndex;
				//Check Local environment variable
				if((localEnvVarIndex=findLocalEnvVar(localEnvVariables,localEnvVariablesCount,commandStruct->argv[argc]+1))!=-1){
					commandStruct->argv[argc]=localEnvVariables[localEnvVarIndex]->value;
				}else if(strcmp(commandStruct->argv[argc]+1,"PATH")==0){
    				//Ready Arguments for the export command
					commandStruct->argv[argc]=buildExportPath(pathArr,*pathArrCount);
				}//Checks Global Environment Variable
				else if((EnvVarValue=getenv(commandStruct->argv[argc]+1))!=NULL){
					commandStruct->argv[argc] =EnvVarValue;//Assign value of variable to arg position
				}
    		}
    	}
        
        //Checks if & passed as arg separately
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
            //Checks if & passed with command
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
	char *return1;
	char * return2;
    
	//Make a copy of the command entered in commandName2
    commandName2 = (char *)malloc(strlen(commandName)+1);
    strcpy(commandName2,commandName);
	
    if (commandName[0] == '.' && commandName[1] == '/') {
        
		//printf("testing2");
		//printf("%s", strcat(getenv("pwd"), &commandName2[1]) );
		return2 = (char *) malloc (sizeof(( strcat(getenv("PWD"), "/" )) ));
		return2 = strcat(getenv("PWD"), "/" );
        
		return1 = (char *) malloc (sizeof(( strcat(return2, &commandName2[2] )) ));
		return1 = strcat(return2, &commandName2[2] );
		//printf("%s", return1);
		//printf("testing");
		//printf("%s", getenv("PWD"));
		return(return1);
	}
    
    
	//Check if the command starts with an / if so return as its already an absolute path
    if (commandName[0] == '/')
    {
        commandName2 = strsep(&commandName2, " ");
        if (stat(commandName2,&st)== 0)
			return(commandName2);
        else
            return("not found");
    }else if(strlen(commandName2)==0){
    	return("not found");
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
    
    char *getwd = (char *) get_current_dir_name();
    char *pgetwd = (char *) malloc (strlen(getwd));
    strcpy(pgetwd,getwd);
    
    char *homedir = getenv("HOME");
    char *pgethd = (char *) malloc (strlen(homedir));
    strcpy(pgethd, homedir);
    char *printDir;
    if (memcmp (pgethd,pgetwd,strlen(homedir)) == 0){
        printDir = (char *) malloc (sizeof(char)*(strlen(pgetwd) - strlen(pgethd) + 2));
        printDir[0] = '~';
        int x;
        for (x = 0; x < strlen(pgetwd) - strlen(pgethd); x++ ){
            printDir[x+1] = pgetwd[(strlen(pgethd))+x];
        }
        printDir[x+1] = '\0';
    }
    else{
        printDir = (char *) malloc (sizeof(char)*strlen(getwd));
        strcpy(printDir,pgetwd);
    }
    gethostname(compName, 80); //Get the name of the computer

	printf ("Commands Entered %d -%s@%s:%s$ ", commandCount, getlogin(), compName, printDir);

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
 * Notes: modifying pathArr without returning & assigning it messes up memory somehow?-wTf
 */
char** addPath(char** pathArr, int* pathArrSize, char* path){
	if(path==NULL){
		exit(EXIT_FAILURE);//Null path
	}

	char* addedPath=malloc(sizeof(char*));
	strcpy(addedPath,path);
	if((findPath(pathArr, pathArrSize, addedPath)!=-1)){
		return pathArr;
	}
	if((pathArr=(char **) realloc(pathArr, sizeof(char *)*((*pathArrSize)+1)))==NULL){
		exit(EXIT_FAILURE);//Alloc Failed
	}
	pathArr[*pathArrSize]=addedPath;
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
 * Results: pathArr will be used to build the "PATH" environment variable
 * Returns: PATH's value as a string
 * http://www.cplusplus.com/forum/beginner/48228/
 */
char* buildExportPath(char** pathArr, int pathArrSize){
	int pathSize=0;
    
	//Calculate the characters from paths
	int i;
	for(i=0;i<pathArrSize; i++){
		pathSize+=strlen(pathArr[i]);
	}
	//Calculate the characters from delimiters
	pathSize+=(pathArrSize-1);
    
	//Allocate memory for resulting path
	char* systemPath=malloc(pathSize);
    
	//Build resulting path to be written
	systemPath[0]=0;//Clear first byte so strcat always sees as empty string
	for(i=0;i<pathArrSize; i++){
		strcat(systemPath,pathArr[i]);
		//If not the last argument then append delimiter
		if(i!=pathArrSize-1){
			strcat(systemPath,":");
		}
	}
	return systemPath;

}

