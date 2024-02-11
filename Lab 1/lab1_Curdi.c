#include "commands.h"

// global variables
NODE *root; 
NODE *cwd;
char *cmd[] = {"mkdir", "rmdir", "cd", "ls", "pwd", "creat", "rm", "save", "reload", "quit", 0};  // fill with list of commands


// finds and returns the index of a command in the commands array
int findCommand(char *command) {
	int i = 0;
	// while we haven't reached the end of commands
	while(cmd[i]) {
		// if the user entered command == the current command - return the index
		if (strcmp(command, cmd[i]) == 0)
			return i;
		i++;
	}
	return -1;
}

//initializes the root node of the file tree and current working directory
int initialize() {
	root = (NODE *)malloc(sizeof(NODE)); // allocate space for a node
	strcpy(root->name, "/"); // set the name of the item to "/" for root directory
	root->parent = root; // set it's parent to itself
	root->sibling = 0; // initialize sibling and child to 0
	root->child = 0;
	root->type = 'D'; // type = directory
	cwd = root; // save it as the current working directory
	printf("Filesystem initialized!\n");
}


int main() {
	// initialize the file system
	initialize();
	char userInput[MAXLINELENGTH];
	char *command;
	int commandIndex;
	char *arg;
	
	// program loop
	while(1) {
		// get user input
		printf("Enter command: ");
		fgets(userInput, sizeof(userInput), stdin);

		// parse user input ("command arg\n")
		command = strtok(userInput, " "); // parse the command
		// if the user did not enter an arg, we need to remove the newline char from the command
		int newLineIndex = strcspn(command, "\n"); // get the index of the newline
		command[newLineIndex] = 0; // replace the newline with null 
		arg = strtok(NULL, "\n"); // parse the arg
		//printf("command: %s, arg: %s\n", command, arg);
		
		// find the command
		commandIndex = findCommand(command);

		// run the command
		switch(commandIndex) {
			case 0: // mkdir
				mkdir(cwd, arg);
				break;
			case 1: // rmdir
				rmdir(cwd, arg);
				break;
			case 2: // cd
				cd(&cwd, arg);
				break;
			case 3: // ls
				ls(cwd);
				break;
			case 4: // pwd
				pwd(cwd);
				break;
			case 5: // creat
				creat(cwd, arg);
				break;
			case 6: // rm
				rm(cwd, arg);
				break;
			case 7: // save
				save(root, arg);
				break;
			case 8: // reload
				reload(root, arg);
				break;
			case 9: // quit
				quit(root);
				break;
			default: // default error message
				printf("Command not found!\n");
		}
	}
}
