#include<stdio.h>
#include<stdlib.h>
#include<string.h>
char** split_line(char *line,char *parse)
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
  //int i=0;
  //while(tokens[i]!=NULL) {printf("token%d:%s\n",i,tokens[i]);i++;}
  return tokens;
}
