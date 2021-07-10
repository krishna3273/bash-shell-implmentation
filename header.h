#include<stdio.h>
#include<stdlib.h>
#include <sys/utsname.h> 
#include<string.h>
#include <signal.h>
#include <sys/wait.h>
#include<time.h>
#define RED  "\x1B[31m"
#define GREEN  "\x1B[1;32m"
#define BLUE  "\x1B[1;34m"
#define WHITE  "\x1B[37m"
#define RESET "\x1B[0m"
#define BUFSIZE 64
#define parse_array " \t\r\n\a"

struct utsname unameData;
char* user;
char cwd[1024];
char mcwd[1024];
char home[1024];
int pid_process[1000];
int num=0;
int fore=1;
//int child[30];
//char* name[30];
