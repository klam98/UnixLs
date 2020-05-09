#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

/*************************************OPTION FUNCTIONS*************************************/

// ls -i option
void printInode(struct stat fileStats);

// ls -l option
void printLong(struct stat fileStats);

// ls -il or ls -li option
void printInodeAndLong(struct stat fileStats);

/*************************************HELPER FUNCTIONS*************************************/

void getAndPrintGroup(gid_t grpNum);

void getAndPrintUserName(uid_t uid);

bool hasWhiteSpace(char *file_name);