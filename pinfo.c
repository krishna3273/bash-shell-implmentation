#include "stdio.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/wait.h"
#include "sys/types.h"
#include "stdlib.h"
#include "split_arg.c"
#include "chd.c"

/*char** split_line1(char *line,char *parse)
{
  int bufsize = BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, parse);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, parse);
  }
  tokens[position] = NULL;
  return tokens;
}*/

char* readstat(char* pid1,int num){
  char dir[60]="/proc/";
  strcat(dir,pid1);
  strcat(dir,"/stat");
  char* content=(char*)malloc(sizeof(char)*3000);
  int fd = open(dir, O_RDONLY);
  if(fd == -1){
    perror("error open file");
    return "abc";
  }
  read(fd, content, 1000);
  char** stat = split_line(content,parse_array );
  return stat[num];

}
int pinfo1(char **args){
  char pid[10];
  if(args[1]==NULL){
    sprintf(pid, "%d", getpid());
  } else {
    strcpy(pid, args[1]);
  }
  char exeP[500];
  char symP[200]="/proc/";
  strcat(symP,pid);
  strcat(symP, "/exe");
  int t = readlink(symP, exeP, sizeof(exeP));
  if(t == -1){
    strcpy(exeP, "broken link");
  }
  exeP[t] = '\0';
  convert(exeP);
  int size=atoi(readstat(pid,22));
  size=size/1024;
  printf("pid -> %s\nProcess Status -> %s\nVirtual Memory-> %dkB\nExecutable Path -> %s\n", readstat(pid,0), readstat(pid,2), size, exeP);
  return 1;
}
