#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// max line length for user input in the terminal
#define MAXLINELENGTH 255

typedef struct node {
	char  name[64];       // node's name string
	char  type;
	struct node *child, *sibling, *parent;
} NODE;


// main command functions
void mkdir(NODE *cwd, char *pathName);
void rmdir(NODE *cwd, char *pathName);
void cd(NODE **cwd, char *pathName);
void ls(NODE *cwd);
void pwd(NODE *cwd);
void creat(NODE *cwd, char *pathName);
void rm(NODE *cwd, char *pathName);
void save(NODE *root, char *fileName);
void reload(NODE *root, char *fileName);
void quit(NODE *root);

// takes an absolute path and returns a pointer to the parent of the path
NODE *navigateToAbsolutePath(NODE *cwd, char **pathName);
// helper for mkdir() and creat()
void createFile(NODE *cwd, char *fileName, char type);
// helper for rmdir() and rm()
void removeFile(NODE *cwd, char *fileName, char type);
// recursively builds a char * containing the absolute path to cwd. returns via output parameter
void getAbsolutePathRecursive(NODE *cwd, char *absolutePath);
// helper for save(). recursively traverses the file tree and saves all node data
void saveFileTreeRecursive(NODE *root, FILE *outfile);