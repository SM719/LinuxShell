#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_ARGS 64

#define MAX_ARG_LEN 16

#define MAX_LINE_LEN 80

#define WHITESPACE " ~,\t\n"

struct command_t {
char *name;
int argc;
char *argv[MAX_ARGS];
};

int runInBackground = 0;

int changestdout = 0;

char *filename;
struct command_t command;



//Function Declarations 
void printPromptMessage(void);
void readUserCommand(char *userCommandEntered);
int parseCommandEntered(char *userCommandEntered, struct command_t *commandStruct);
void checkIfUserChangesDirectory(char *userCommandEntered);

int main(int argc, char *argv[]) {

	int pid, numChildren, status, count;
	struct command_t command; // Shell initialization
	char userCommandEntered[80];

	
	while(strcmp (userCommandEntered, "exit\n") != 0) {	
		printPromptMessage();

		readUserCommand(userCommandEntered);
		checkIfUserChangesDirectory(userCommandEntered);
		
		if((pid = fork()) == 0) {
		/* Child executing command */

		//execv(path, command.argv);				
		//execv(command.name, command.argv);
			
		}
		

		//printf("\nteseting\n");
	}
	return 0;
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

void checkIfUserChangesDirectory(char *userCommandEntered){
	

//IF STATEMENTS TO CHECK IS USER WANTS TO NAVIGATE TO FOLDER IN HOME DIR BY USING '~'
        if (userCommandEntered[0] == 'c' && userCommandEntered [1] == 'd' && userCommandEntered[2] == ' ' && userCommandEntered[3] == '~' && userCommandEntered[4] == '/')
        {
            parseCommandEntered(userCommandEntered, &command);
			
			//CALL SYSTEM FUNCTION TO CHANGE CURRENT DIRECTORY WITH ARGUMENT 'ABSOLUTE PATH TO FOLDER'
            int ret = chdir(strcat(getenv("HOME"), command.argv[2]));
			if( ret != 0 )
				printf("Error: %s\n", strerror(errno)); 

        }
        //IF STATEMENT TO CHECK IF USER WANTS TO NAVIGATE TO THE HOME FOLDER BY USING '~'
        else if (userCommandEntered[0] == 'c' && userCommandEntered [1] == 'd' && userCommandEntered[2] == ' ' && userCommandEntered[3] == '~')
        {
            int i = 2;
            char *homedir;
			//CALL SYSTEM FUNCTION TO GET HOME DIRECTORY AND PASS THIS AS ARGUMENT TO CHANGE CURRENT WORKING DIRECTORY   
            homedir = getenv ("HOME");
            i = chdir(homedir);
        }
        //IF STATEMENT TO CHECK IS USER WANTS TO GO UP ONE DIRECTORY LEVEL BY USING 'cd ..'
        else if (userCommandEntered[0] == 'c' && userCommandEntered [1] == 'd' && userCommandEntered[2] == ' ' && userCommandEntered[3] == '.' && userCommandEntered[4] == '.')
        {
            int loop;
            int i;
            char *cwd = (char *) get_current_dir_name();
            int cwdlength = (strlen (cwd)) - 1;
            char *newcwd = (char *) malloc(cwdlength+1);
            //ITERATE THRU THE LOOP STARTING AT THE LAST ELEMENT OF CURRENT DIRECTORY AND STOP AFTER FINDING THE FIRST '/'. 
			//PASS THIS NEW STRING AS AN ARGUMENT TO SYSTEM CALL FOR CHANGING WORKING DIRECTORY. 
			//IF THE FIRST OCCURENCE OF '/' IS AT THE FIRST CHARACTER THEN CHANGE DIRECTORY TO ROOT.   
            for (loop = cwdlength - 1; loop >= 0; --loop)
            {
                if (loop == 0)
                {
                    i = chdir("/");
                    break;
                }
                else if (cwd[loop] == '/')
                {               
                    strxfrm(newcwd,cwd,loop);
                    i = chdir(newcwd);
                    break;   
                }
            }
        }



}

