#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include "UnixLs.h"

#define EXIT_FAILURE 1

/*
	Implement only the following options:
	1. -i
	2. -l

	Be sure to test every option you have implemdped and all the permutations of the various options 
	(e.g. -il, -i, -li, -l, etc.) Of course, don't forget the most obvious test of all, no options.

	To simplify the printing of dates, you are to use the format
	mmm dd yyyy hh:mm
	e.g. Oct 2 2019 13:32

	When performing a long listing (-l) there is no need to print the line that begins with "total".
*/

/*************************************HELPER FUNCTIONS*************************************/

char *getUserName(uid_t uid) {
  struct passwd *pw = NULL;
  pw = getpwuid(uid);

  if (pw) {
    return pw->pw_name;
  } 
  else { 
    return "null";
  }
}

char *getGroupName(gid_t grpNum) {
  struct group *grp;
  grp = getgrgid(grpNum); 

  if (grp) {
    return grp->gr_name;
  } 
  else {
    return "null";
  }
}

bool hasWhiteSpace(char *file_name) {
  for (int i = 0; i < strlen(file_name - 1); i++) {
    if (isspace(file_name[i])) {
      return true;
    }
  }
  return false;
}

/*************************************OPTION FUNCTIONS*************************************/

void printInode(struct stat fileStats) {
  printf("%20lu ", fileStats.st_ino);
}

void printLong(struct stat fileStats) {
// first check if it is a directory or file
  if (S_ISDIR(fileStats.st_mode)) {
    printf("d"); // its a directory
  }
  else {
    printf("-"); // its a file
  }

  // owner permissions
  if (fileStats.st_mode & S_IRUSR) { // USER->READ
    printf("r");
  }
  else {
    printf("-");
  }

  if (fileStats.st_mode & S_IWUSR) { // USER->WRITE
    printf("w");
  }
  else {
    printf("-");
  }

  if (fileStats.st_mode & S_IXUSR) { // USER->EXECUTE
    printf("x");
  }
  else {
    printf("-");
  }

  // group permissions
  if (fileStats.st_mode & S_IRGRP) { // GROUP->READ
    printf("r");
  }
  else {
    printf("-");
  }

  if (fileStats.st_mode & S_IWGRP) { // GROUP->WRITE
    printf("w");
  }
  else {
    printf("-");
  }

  if (fileStats.st_mode & S_IXGRP) { // GROUP->EXECUTE
    printf("x");
  }
  else {
    printf("-");
  }

  // other users permissions
  if (fileStats.st_mode & S_IROTH) { // OTHER USERS->READ
    printf("r");
  }
  else {
    printf("-");
  }

  if (fileStats.st_mode & S_IWOTH) { // OTHER USERS->WRITE
    printf("w");
  }
  else {
    printf("-");
  }

  if (fileStats.st_mode & S_IXOTH) { // OTHER USERS->EXECUTE 
    printf("x ");
  }
  else {
    printf("- ");
  }

  // grab username and group names using helper methods
  char *username = getUserName(fileStats.st_uid);
  char *groupname = getGroupName(fileStats.st_gid);

  // grab timestamp of last modification to a file/directory
  char *timestamp = ctime(&fileStats.st_mtime);
  timestamp[strlen(timestamp) - 1] = '\0'; // stop function from going to next line

  // print in the following order: # hard links | username | group name | file size | timestamp of last modification | file/directory name
  printf("%lu %4s %4s %7lu %s ", fileStats.st_nlink, username, groupname, fileStats.st_size, timestamp);
}

/******************************************DRIVER*****************************************/

// implementation checklist:
// 1. handle multiple directories

int main(int argc, char *argv[]) {
  DIR *dir;
  struct dirent *dp;
  struct stat buf;
  char *file_name;

  // only ./UnixLs was entered, therefore ls is run on the cwd "."
  if (argc == 1) {
    dir = opendir(".");

    while ((dp = readdir(dir)) != NULL) {
      if (dp->d_name[0] != '.') {
        file_name = dp->d_name;

        // adding single quotes to file names that have white spaces in them (like real ls)
        if (hasWhiteSpace(file_name)) {
          printf("'%s'\n", file_name);
        }
        else {
          printf("%s\n", file_name);
        }
      }
    }

    closedir(dir);
  }

  // must check for [OPTION], [PATH], etc.
  if (argc >= 2) {
    char *option_or_path = argv[1];
    char *path = (char *)malloc(255 * sizeof(char));
    char *fullpath = (char *)malloc(255 * sizeof(char));
    char *filepath;
    char *file_name;
    char *link_name;
    ssize_t r;

    // initializing an array of strings to hold each path given
    // char directoryArray[argc - 2][255];

    // for (int i = 2; i < argc; i++) {
    //   strcpy(directoryArray[i - 2], argv[i]);
    //   directoryArray[i - 2] + '\0';
    // }

    // for (int i = 0; i < (sizeof(directoryArray) / sizeof(directoryArray[0])); i++) {
    //   printf("directoryArray[%d]: %s\n", i, directoryArray[i]);
    // }

    // running program as "./UnixLs ls" will not work (not sure if this was how as4 was supposed to be implemented)
    if (!strcmp(option_or_path, "ls")) {
      printf("ERROR: invalid formatting, this program runs by executing ./UnixLs [OPTION] [PATH].\n");
      printf("EXAMPLE: ./UnixLs -il ..\n");
      exit(EXIT_FAILURE);
    }

    // checking that -i, -l, -il, or -li was used
    if (option_or_path[0] == '-' && (option_or_path[1] == 'i') || option_or_path[1] == 'l') {
      // check for edgecases "./UnixLs -i -l [PATH]" or "./UnixLs -l -i [PATH]"
      if (argc >= 3) {
      	char *check_il_or_li_edgecase = argv[2];
      	if (!strcmp(check_il_or_li_edgecase, "-i") || !strcmp(check_il_or_li_edgecase, "-l")) {
      		strncat(option_or_path, &check_il_or_li_edgecase[1], 1);
      	}
      }

      // [PATH] was not given, therefore do ls with [OPTION] on cwd
      if (!strcmp(argv[argc - 1], option_or_path)) {
      	filepath = ".";
      }
      else {
        // copy [PATH] to filepath
        strcpy(path, argv[argc - 1]);
        filepath = path;
      }
    }
    else { 
      // no [OPTION] specified, just copy the path
      strcpy(path, option_or_path);
      filepath = path;
    }

    dir = opendir(filepath);

    // illegal path; doesn't contain '/', '~', '.', '..'
    if (dir == NULL) {
      printf("File path [%s] was not found!\n", filepath);
      exit(EXIT_FAILURE);
    }
    else {
      while ((dp = readdir(dir)) != NULL) {
      strcpy(fullpath, filepath);
      strcat(fullpath, "/");
      strcat(fullpath, dp->d_name);

      file_name = dp->d_name;

      // file exists
      if (stat(fullpath, &buf) == 0) {
        if (file_name[0] != '.') {
          if (option_or_path[1] == 'i' && option_or_path[2] != 'l') {
            printInode(buf);
          }
          else if (option_or_path[1] == 'l' && option_or_path[2] != 'i') {
            printLong(buf);
          }
          else if ((option_or_path[1] == 'i' && option_or_path[2] == 'l') 
                    || (option_or_path[1] == 'l' && option_or_path[2] == 'i')) {
            printInode(buf);
            printLong(buf);
          }

          // adding single quotes to file names that have white spaces in them (like real ls)
          if (hasWhiteSpace(file_name)) {
            printf("'%s'", file_name);
          }
          else {
            printf("%s", file_name);
          }

          // handle symbolic link
          if (lstat(fullpath, &buf) == -1) {
            printf("\n");
          }
          else {
            link_name = malloc(buf.st_size + 1);
            r = readlink(fullpath, link_name, buf.st_size + 1);

            if (r < 0 || r > buf.st_size) {
              printf("\n");
            }
            else {
              link_name[buf.st_size] = '\0';
              printf(" -> %s\n", link_name);
            }

            free(link_name);
          }
        }
      }
      else {
          printf("Stat failed: %s\n\n", strerror(errno));
        }
      }
    }

    free(path);
    free(fullpath);
    closedir(dir);
  }

  return 0;
}

