#include "stdio.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "malloc.h"
#include "string.h"
#include "dirent.h"
#include "time.h"
#include "pwd.h"
#include "grp.h"
char date[100];
void printpermissions(struct stat dir,struct dirent* name){
  strftime(date, 100, "%h %d %H:%M", localtime(&(dir.st_ctime)));
        printf( (S_ISDIR(dir.st_mode)) ? "d" : "-");
        printf( (dir.st_mode & S_IRUSR) ? "r" : "-");
        printf( (dir.st_mode & S_IWUSR) ? "w" : "-");
        printf( (dir.st_mode & S_IXUSR) ? "x" : "-");
        printf( (dir.st_mode & S_IRGRP) ? "r" : "-");
        printf( (dir.st_mode & S_IWGRP) ? "w" : "-");
        printf( (dir.st_mode & S_IXGRP) ? "x" : "-");
        printf( (dir.st_mode & S_IROTH) ? "r" : "-");
        printf( (dir.st_mode & S_IWOTH) ? "w" : "-");
        printf( (dir.st_mode & S_IXOTH) ? "x" : "-");
        printf(" %d ", (int)dir.st_nlink);
        struct group  *gr = getgrgid(dir.st_gid);
        struct passwd *ur = getpwuid(dir.st_uid);
        printf(" %s", ur->pw_name);
        printf(" %s", gr->gr_name);
        printf(" %s   ", date);
        printf("%d", (int)dir.st_size);
        printf(" %s", name->d_name);
        printf("\n");
}

int ls1(char **args){
  //printf("krishna");
 DIR *dir;
  struct dirent *name;
  struct stat directory;
  //struct stat opening;

  int a = 0, l = 0, count = 0, j = 0;

  for (int i = 1; args[i] != NULL; i++) {
    if (strcmp(args[i], "-a") == 0)
      a = 1;
    else if (strcmp(args[i], "-l") == 0)
      l = 1;
    else if (strcmp(args[i], "-al") == 0 || strcmp(args[i], "-la") == 0){
      a = 1;
      l = 1;
    }
    else if(strcmp(args[i], " ") != 0) count++;
  }
  int validentry = 0;
  if (count == 0) {
    count = 1;
    validentry = 1;
  }
  char **ls_req = (char **)malloc(count * 8);
  for (int i = 0; i < count; i++) {
    ls_req[i] = (char *)malloc(50);
  }
if (validentry) ls_req[0] = ".";
  for (int i = 1; args[i] != NULL; i++) {
    if (!(strcmp(args[i], "-al") == 0 || strcmp(args[i], "-la") == 0 ||strcmp(args[i], "-a") == 0 || strcmp(args[i], "-l") == 0)){
      ls_req[j] = args[i];
      j++;
    }
  }
  for (int i = 0; i < count; i++){
    dir = opendir(ls_req[i]);
    if (dir == NULL) {
      printf("%s: No such file or directory\n", ls_req[i]);
      continue;
    }
    while((name = readdir(dir)) != NULL){
   stat(name->d_name, &directory);
      if (!a && !l) {
        if(name->d_name[0] != '.')
        printf("%s\n", name->d_name);
      }
      else if (!l && a) {
        printf("%s\n", name->d_name);
      }
      else if (a && l) {
        printpermissions(directory,name);
      }
      else {
        if(name->d_name[0] != '.') {
          printpermissions(directory,name);
        }
      }
    }
  }
  closedir(dir);

  return 1;
}
