#include "commands.h"


/*
    mkdir pathname
    Make a new directory for a given pathname.
    Show error message (DIR pathname already exists!) if the directory already present in the
    filesystem.
*/
void mkdir(NODE *cwd, char *pathName) {
    // if user didn't enter a filename
    if (pathName == NULL) {
        printf("Too few arguments!\n");
        return;
    }

    // create a new file if type directory
    createFile(cwd, pathName, 'D');
}

/*
        rmdir pathname
    Remove the directory, if it is empty.
    Show error messages if:
    − The directory specified in pathname does not exist (DIR pathname does not exist!)
    − The directory is not empty (Cannot remove DIR pathname (not empty)!).
*/
void rmdir(NODE *cwd, char *pathName) {
    // if user didn't enter a pathname
    if (pathName == NULL) {
        printf("Too few arguments!\n");
        return;
    }

    // call remove file with type directory
    removeFile(cwd, pathName, 'D');
}

/*
    cd [pathname]
    Change CWD to pathname, or to / if no pathname specified.
    Display an error message (No such file or directory: pathname) for an invalid pathname.
*/
// NOTE: we need to pass in a pointer to the cwd pointer to save the modifications outside the scope of the function
void cd(NODE **cwd, char *pathName) {
    // if user didn't enter a filename - go to root
    if (pathName == NULL) {
        // iterate to root
        while ((*cwd)->parent != *cwd) { // root points to itself, so we can check for that
            (*cwd) = (*cwd)->parent;
        }
        return;
    }

    // if the user wants to go back a level - go to parent of cwd
    if (strcmp(pathName, "..") == 0) {
        *cwd = (*cwd)->parent;
        return;
    }

    // go to child of cwd
    NODE *pCur = (*cwd)->child; // we need to dereference the double pointer to access the value
    // iterate to correct sibling
    while (pCur != NULL) {
        // if found
        if (strcmp(pCur->name, pathName) == 0) {
            // if it is just a file
            if (pCur->type == 'F') {
                printf("%s is a file, not a directory.\n", pathName);
                return;
            }

            // else - enter the directory
            *cwd = pCur;
            return;
        }
        // shift
        pCur = pCur->sibling;
    }

    printf("No such directory: %s\n", pathName);
}

/*
    ls [pathname]
    List the directory contents of pathname or CWD (if pathname not specified).
    Display an error message (No such file or directory: pathname) for an invalid pathname.
*/
void ls(NODE *cwd) {
    // go to child of cwd
    NODE* pCur = cwd->child;

    // traverse each sibling
    while (pCur != NULL) {
        printf("%c %s\n", pCur->type, pCur->name);
        pCur = pCur->sibling;
    }

    // when pCur == NULL - there are no more siblings to print
}

/*
    pwd
    Print the (absolute) pathname of CWD.
*/
void pwd(NODE *cwd) {
    // allocate space for a char *
    char *absolutePath = (char*)malloc(MAXLINELENGTH);

    // compute the absolute path (recieve the value as an output parameter)
    getAbsolutePathRecursive(cwd, absolutePath);

    // print the absolute path
    printf("%s\n", absolutePath);
}

/*
    creat pathname
    Create a new FILE node.
    Show error message (pathname already exists!) if the directory already present in the filesystem.
*/
void creat(NODE *cwd, char *pathName) {
    // if user didn't enter a filename
    if (pathName == NULL) {
        printf("Too few arguments!\n");
        return;
    }

    // create a new file if type file
    createFile(cwd, pathName, 'F');
}

/*
    rm pathname
    Remove the FILE node specified by pathname.
    Display an error message (File pathname does not exist!) if there no such file exists.
    Display an error message (Cannot remove pathname (not a FILE)!) if pathname is not a
    FILE.
*/
void rm(NODE *cwd, char *pathName) {
    // if user didn't enter a pathname
    if (pathName == NULL) {
        printf("Too few arguments!\n");
        return;
    }

    // call remove file with type directory
    removeFile(cwd, pathName, 'F');
}

/*
    save filename
    Save the current filesystem tree in the file filename.
*/
void save(NODE *root, char* fileName) {
    // if no file name was passed as an arg - give default file name
    if (fileName == NULL) {
        char *defaultFileName = "ffsim-curdi.txt";
        fileName = malloc(strlen(defaultFileName) + 1);
        strcpy(fileName, defaultFileName);
    }

    // open a file stream
    FILE *outfile = fopen(fileName, "w+");

    // check for file open failure
    if (outfile == NULL) {
        printf("Failed to open file: %s\n", fileName);
        return;
    }

    // call helper to recursively traverse the tree and save all node data
    saveFileTreeRecursive(root, outfile);

    // close the file
    fclose(outfile);
}

/*
    reload filename
    Re-initalize the filesystem tree from the file filename.
*/
void reload(NODE *root, char *fileName) {
	char fileLine[MAXLINELENGTH];

    // if no file name was passed as an arg - give default file name
    if (fileName == NULL) {
        char *defaultFileName = "ffsim-curdi.txt";
        fileName = malloc(strlen(defaultFileName) + 1);
        strcpy(fileName, defaultFileName);
    }

    // open a file stream
    FILE *infile = fopen(fileName, "r");

    // check for file open failure
    if (infile == NULL) {
        printf("Failed to open file: %s\n", fileName);
        return;
    }

    // burn the first line
    fgets(fileLine, sizeof(fileLine), infile);

    // load in the file tree
    while (fgets(fileLine, sizeof(fileLine), infile)) { // current line exists
        // fileLine now holds the current line
        // parse type and path ("D /path/to/file\n")
        char *type = strtok(fileLine, " ");
        char *path = strtok(NULL, "\n");

        // if type directory
        if (strcmp(type, "D") == 0) {
            // call mkdir
            mkdir(root, path);
        }

        // if type file
        else if (strcmp(type, "F") == 0) {
            // call creat
            creat(root, path);
        }
    }

    // close the file
    fclose(infile);
}

/*
    quit
    Save the filesystem tree in filename fssim lastname.txt, then terminate the program.
    “lastname” is your surname.
*/
void quit(NODE *root) {
    // save
    save(root, "ffsim-curdi.txt");
    // exit the program
    exit(0);
}


// takes an absolute path and returns a pointer to the parent of the path
// if path doesnt exist, returns a null pointer
NODE *navigateToAbsolutePath(NODE *cwd, char **pathName) {
    /*
        Just need to edit pathName down to the actual new file name,
        and return a pointer to the new file's parent
    */

    // start at the root node
    while (cwd->parent != cwd) { // root points to itself, so we can check for that
        cwd = cwd->parent;
    }

    // skip the first '/' in the pathname
    if ((*pathName)[0] == '/') (*pathName)++;

    // parse the entire path into individual nodes
    char *nodes[20] = {NULL}; // initialize with null pointers
    char *token = strtok(*pathName, "/"); // parse the initial node
    int i = 0;
    while (token != NULL && i < 20) {
        nodes[i] = strdup(token); // duplicate the node into the nodes array
        token = strtok(NULL, "/"); // get the next node in the path
        i++;
    }

    // traverse each part of the path
    i = 0;
    while (nodes[i] != NULL && i < 19) {
        // if were at the second last node in the path: return this node as the cwd. the new file will be inserted as a child of this dir
        if (nodes[i + 1] == NULL) {
            // update pathName
            strcpy(*pathName, nodes[i]); // return the actual file name
            // return updated cwd
            return cwd;
        }

        // go to child of cwd
        cwd = cwd->child;

        // search siblings for next node
        while (cwd != NULL) {
            // if found and type D
            if (strcmp(cwd->name, nodes[i]) == 0 && cwd->type == 'D') break;

            // shift
            cwd = cwd->sibling;
        }

        // cwd now points to current node in path or null. if null - bail
        if (cwd == NULL) return cwd;

        i++;
    } 

    // return the updated cwd
    return cwd;
}

// helper for mkdir() and creat()
void createFile(NODE *cwd, char *fileName, char type) {
    // if path is absolute
    if (fileName[0] == '/') {
        // get the cwd to insert the new file into
        cwd = navigateToAbsolutePath(cwd, &fileName);
        // fileName is modified as an output parameter, and now holds the name of the new file
        // cwd now points to the parent of the desired destination
        // Note: changes to cwd will not be retained outside the scope of this function, 
        // so the node will be inserted, but the user will still be in the same spot

        // if function returned null - path doesn't exist
        if (cwd == NULL) {
            printf("Path does not exist!\n");
            return;
        }
    }

    NODE *pCur = cwd;
    NODE *newFile;

    // case: there are no files in the cwd - insert here
    if (pCur->child == NULL) {
        // allocate space for new node
        newFile = (NODE*)malloc(sizeof(NODE)); 
        // if memory allocation failed - bail on the function
        if (newFile == NULL) {
            printf("Error: memory allocation failed!\n");
            return;
        }
        // link the new node to it's parent
        cwd->child = newFile;
        newFile->parent = cwd;
    }

    // case: there are files in the cwd
    else {
        // go to cwd->child
        pCur = pCur->child;

        // iterate through siblings
        while (pCur->sibling != NULL) {

            // if name already exists - print error
            if (strcmp(pCur->name, fileName) == 0) {
                if (type == 'D') printf("DIR %s already exists!\n", fileName);
                if (type == 'F') printf("File %s already exists!\n", fileName);
                return;
            }

            // shift to next sibling
            pCur = pCur->sibling;
        }

        // check the last sibling manually
        if (strcmp(pCur->name, fileName) == 0) {
            if (type == 'D') printf("DIR %s already exists!\n", fileName);
            if (type == 'F') printf("File %s already exists!\n", fileName);
            return;
        }

        // pCur->sibling == NULL - so insert here
        // allocate space for new node
        newFile = (NODE*)malloc(sizeof(NODE)); 
        // if memory allocation failed - bail on the function
        if (newFile == NULL) {
            printf("Error: memory allocation failed!\n");
            return;
        }
        // link the new node to it's sibling
        pCur->sibling = newFile;
        newFile->parent = cwd; // lparent of the n-th sibling will point to the actual parent (cwd)
    }

    // regardless of which case executed above, update rest of new node
    strcpy(newFile->name, fileName); // set the file name
    newFile->type = type;
    newFile->sibling = NULL;
    newFile->child = NULL;
}

// helper for rmdir() and rm()
void removeFile(NODE *cwd, char *fileName, char type) {
    NODE *pPrev, *pCur;

    // first, go to cwd->child
    pPrev = cwd;
    pCur = cwd->child;

    // iterate through all siblings to find the file
    while (pCur != NULL) {
        // if found
        if (strcmp(pCur->name, fileName) == 0) {
            // error messages for type D 
            if (type == 'D') {
                // if not empty
                if (pCur->child != NULL) {
                    printf("Cannot remove DIR %s (not empty)!\n", fileName);
                    return;
                }

                // if wrong type
                if (pCur->type != 'D') {
                    printf("Cannot remove %s (not a directory)!\n", fileName);
                    return;
                }
            }

            // error messages for type F
            if (type == 'F') {
                // if wrong type
                if (pCur->type != 'F') {
                    printf("Cannot remove %s (not a file)!\n", fileName);
                    return;
                }
            }

            // remove the file (both types can be treated the same here)
            // modify links
            if (pPrev == cwd) {
                // if removing leftmost sibling in level
                pPrev->child = pCur->sibling;
            }
            else {
                // if not leftmost sibling
                pPrev->sibling = pCur->sibling;
            }
            // free memory
            free(pCur);

            // end of function
            return;
        }

        // shift
        pPrev = pCur;
        pCur = pCur->sibling;
    }

    // pCur->sibling == NULL, so there are no more files to search through
    // fileName does not exist - print error message
    if (type == 'D') printf("DIR %s does not exist!\n", fileName);
    if (type == 'F') printf("File %s does not exist!\n", fileName);
}

// recursively builds a char * containing the absolute path to cwd. returns via output parameter
void getAbsolutePathRecursive(NODE *cwd, char *absolutePath) {
    // base case: cwd is head
    if (cwd->parent == cwd) {
        // add "/" to the path
        strcpy(absolutePath, "/");
        return;
    }

    // build the path recursively: </path/to/parent> + </cwd>
    // build path to parent
    getAbsolutePathRecursive(cwd->parent, absolutePath);

    // append the cwd
    // only add a / before the current dir if its parent is not root
    if (strcmp(absolutePath, "/") != 0) {
        strcat(absolutePath, "/");
    }
    strcat(absolutePath, cwd->name);
}

// helper for save(). recursively traverses the file tree (DFS) and saves all node data
void saveFileTreeRecursive(NODE *pCur, FILE *outfile) {
    // base case: pCur is NULL node
    if (pCur == NULL) return;

    // print the current node
    // get path
    char *absolutePath = (char*)malloc(MAXLINELENGTH); // allocate space for a char *
    getAbsolutePathRecursive(pCur, absolutePath); // compute the absolute path (recieve the value as an output parameter)
    // print the line to the file
    fprintf(outfile, "%c %s\n", pCur->type, absolutePath);

    // print subtree
    saveFileTreeRecursive(pCur->child, outfile);

    // print sibling tree
    saveFileTreeRecursive(pCur->sibling, outfile);
}