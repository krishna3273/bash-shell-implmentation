#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void convert(char* input){
  int i, flag=0, len = strlen(input), j;
  for(i=0; home[i]!='\0'; i++) {
    if(input[i]!=home[i]){
      flag = 1;
      break;
    }
  }
  if(flag==1){
    return;
  } else {
    char newCWD[len-i+3];
    newCWD[0]='~';
    for(j=1; i<len; i++, j++){
      newCWD[j] = input[i];
    }
    strcpy(input, newCWD);
  }
}