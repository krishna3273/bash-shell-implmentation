#include "header.h"
#include "ls.c"
#include "pinfo.c"
#include <ctype.h>
#include <signal.h>
int mainpid;
pid_t childpid=-1;
void print_prompt() {
    uname(&unameData);
    user=getenv("USER");
    getcwd(cwd,1024);
    if(!strcmp(cwd,home))
      strcpy(cwd,"~");
    strcpy(mcwd,cwd);
    convert(mcwd);
    fprintf(stdout, GREEN "%s@%s:" RESET,user,unameData.nodename);
    fprintf(stdout, BLUE "%s$" RESET, mcwd);
}

int cd(char **args);
int exit1(char **args);
int pwd();
int myprint(char **args);
int remind(char **args);
int  clock2q(char **args);
int mysetenv(char **args);
int myunsetenv(char **args);
void addjob(int pid);
int printjobs();
int killjob(char** args);
int overkill();
int Fg(char** args);
int Bg(char** args);
void catchC(int sig);
void catchZ(int sig);
char** redirection(char** args,int* in_f,int* out_f,int* in1,int* out1);
//char line[1000];
  //List of builtin commands, followed by their corresponding functions.
char *builtin_str[] = {
  "cd",
  "exit",
  "pwd","pinfo","remindme","clock","jobs","kjob","overkill","fg","bg","setenv","unsetenv"
};


int (*builtin_func[]) (char **) = {
  &cd,
  &exit1,
  &pwd,
  &pinfo1,
  &remind,
  &clock2q,
  &printjobs,
  &killjob,
  &overkill,
  &Fg,
  &Bg,
  &mysetenv,
  &myunsetenv
};


char *myimp_str[] = {
  "ls"
};

int (*myimp_func[]) (char **) = {
  &ls1,
};
int clock2q(char **args){
  int sleep1=atoi(args[2]);
  int upto=atoi(args[4]);
  int p=0;
  char* content=(char*)malloc(sizeof(char)*200);
  while(1){
    if(p>upto-sleep1) break;
    int fd = open("/proc/driver/rtc",O_RDONLY);
			read(fd,content,200);
  char** stat = split_line(content,parse_array );
    printf("%s,%s\n",stat[5],stat[2]);
    if(p==upto-sleep1) break;
    sleep(sleep1);
    p+=sleep1;
    free(stat);
  }
  return 1;
}


int remind(char** args){
    int sleep1=atoi(args[1]);
    int q=2;
    char *result = (char*)malloc(1000*sizeof(char));
    strcpy(result,"Reminder:" );
    while(args[q]!=NULL){
      strcat(result," ");
      strcat(result,args[q]);
      q++;
    }
    result[10]=' ';
    result[strlen(result)-1]='\0';
    pid_t pid=fork();
    if(pid==0){
      sleep(sleep1);
      printf("\n%s\n",result);

    }
    return 1;
}
int builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int myimp(){
  return sizeof(myimp_str) / sizeof(char *);
}

int myprint(char** args){
  if(args[1]!=NULL)
  printf("%s\n",args[1]);
  return 1;
}
int pwd(){
  if(!strcmp(cwd,"~")){
    printf("%s\n",home);
    return 1;
  }

  printf("%s\n",cwd);
  return 1;
}
int cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (!strcmp(args[1],"~"))
      args[1]=home;
      //printf("%s\n",args[1]);
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int exit1(char **args)
{
  return 0;
}




char* read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  getline(&line, &bufsize, stdin);
  return line;
}

void child_handler(int sig)
{
	int status;
	int wpid = waitpid(-1, &status, WNOHANG);
    fore=1;
	if (wpid > 0 && WIFSIGNALED(status) == 0)
  {
		fprintf(stderr, "\nProcess  with pid: %d exited normally\n",wpid);
	}
	if (wpid > 0 && WIFEXITED(status) == 0) {
		fprintf(stderr, "\nProcess  with pid: %d interrupted by signal %d\n", wpid, sig);
	}
}


int launch(char **args)
{
  pid_t  wpid;
  char tpid[10];
  int status,pid;
  int yes=1;
  sprintf(tpid,"%d" , fork());
  pid=atoi(tpid);
    addjob(pid);
    childpid=pid;
  if (pid == 0) {
          
        //    if(fore==0) 
        if(fore==1)setpgid(0,0); 
    //printf("krishna");
    int* in_fd=(int*)malloc(sizeof(int));
    int* out_fd=(int*)malloc(sizeof(int));
    int* out=(int*)malloc(sizeof(int));
    int* in=(int*)malloc(sizeof(int));
    char **newargs = malloc(sizeof(char*) * 100);
    newargs=redirection(args,in_fd,out_fd,in,out);
    if((*in)==1){dup2((*in_fd),0);} 
    if((*out)==1){dup2((*out_fd),1);}
    //printf("krishna");
    //for (int i = 0; i < myimp(); i++) {
    if (execvp(newargs[0],newargs) == -1)
      perror("lsh");
    exit(EXIT_FAILURE);
    
     if(*in) close(*in_fd);
     if(*out) close(*out_fd);
  } 
  else if (pid < 0) {
    perror("lsh");
  }
  else {

    if(fore==0){ 
      printf("[%d] %d\n",num,pid);
    }

    if(fore==1){
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    /*if(fore == 1)
		{
			// wait(NULL);
			waitpid(-1,NULL,WUNTRACED);
		}
		else
		{
			kill(pid, SIGTTIN);
			// kill(pid, SIGTSTP);
			kill(pid, SIGCONT);
			// addtoLL(head, nowProcess, pid, 1);
			//  fprintf(stderr,KMAG "[+] %d %s\n" RESET, pid, nowProcess);
		}*/

  }
    

  return 1;
}


int execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }


  for (i = 0; i < builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return launch(args);
}

char** redirection(char** args,int* in_f,int* out_f,int* in1,int* out1){
  int total=0;
  int in_fd=0;
  int out_fd=1;
  int in=0;
  int out=0;
  int r=0,i=0,init=0;
  char **newargs = malloc(sizeof(char*) * 100);
  while(args[i]!=NULL){
    if(!(strcmp(args[i],"<") && strcmp(args[i],">") && strcmp(args[i],">>"))) r=1; 
    total++;
    i++;
  }
  if(r==0) return args;
  int start=0;
  while(args[start]!=NULL){
    if(strcmp(args[start], "<") == 0)
		{
			in = 1;
			if(total > start+1)
			{
				in_fd = open(args[++start], O_RDONLY);
				if(in_fd == -1)
				{
				printf("Please enter a valid input file\n");
				exit(1);					
				}
			}
			else
			{
				printf( "Please give an input file");
				exit(1);
			}

		}
		else if(strcmp(args[start], ">") == 0)
		{
			out = 1;
			if(total > start + 1)
			{
				out_fd = open(args[++start], O_WRONLY | O_TRUNC | O_CREAT, 0644);
			}
			else
			{
				printf("Please give an output file\n");
			}				
		}
		else if(strcmp(args[start], ">>") == 0)
		{
			out = 1;
			if(total > start + 1)
				out_fd = open(args[++start], O_WRONLY | O_APPEND | O_CREAT, 0644);
			else
			{
				printf("Please give an output file\n");
			}						
		}			
    else{
      newargs[init++]=args[start];
    }
    start++;
  }
  newargs[init]=NULL;
*in_f=in_fd;
*out_f=out_fd;
*in1=in;
*out1=out;
return newargs;
}

int main(int argc, char const *argv[]){
    char **args;
    char **cmdlist;
    char** pipeargs;
    char *line;
    int status=1;
    getcwd(home,1024);
    int stout=dup(1);
    int stin=dup(0);
    int pout=dup(1);
    int pin=dup(0);
    int fildes[2];
    mainpid=getpid();
    signal(SIGTSTP, catchZ);
    signal(SIGINT, catchC);
while(status){
    char** cmdlist;
    print_prompt();
    line=read_line();
    cmdlist=split_line(line,";");
    int i=0;
    childpid=-1;
        while(cmdlist[i]!=NULL){
            // printf("krishna");
            // printf("%s\n",cmdlist[i]);
    char** pipeargs;
            char** pargs=split_line(cmdlist[i],"|");
            i++;
            int j=0;
             
            while(pargs[j]!=NULL){ 
                // printf("after pipe:%s\n",pargs[j]);                
                j++;
            }

            if(j==1){
                for(int z=0;z<j;z++){
                    char **args;
                    // printf("after pipe:%s\n",pargs[z]);   
                    args=split_line(pargs[z],parse_array);
                    //  printf("after pipe:%s\n",pargs[z]);                
                    // if(dup2(pin,0) != 0){
                        // perror("dup2 failed\n");
                    // }
                    /*dup2(pin,0);
                    close(pin);*/

                     /*if(z == j - 1) pout = dup(stout);
                    else{
                        //int fildes[2];
                        if(pipe(fildes) < 0){
                            perror("pipes failed");
                        }
                        pout = fildes[1];
                        pin = fildes[0];
                    }
                    // if(dup2(pout,1) != 1){
                        // perror("dup2 failed\n");
                    // }
                    dup2(pout,1);
                    close(pout);*/
                    int r=0;
                    // fore=1;
                    while(args[r]!=NULL){
                        //   printf("%s km",args[r]);
                        if(!strcmp(args[r],"&")) {
                            signal(SIGCHLD,child_handler);
                            fore=0;
                            args[r]=NULL;
                            break;
                        }
                        r++;
                    }
                    // printf("\n");
                    // printf("yes");
                    status=execute(args);
                    free(args);
                }
                /*dup2(stin,0);
                close(stin);
                dup2(stout,1);
                close(stout);*/
            }
//-------------------------------------------------------------------------------------------------------------------------
                else{
                    int ppid=fork();
                    if(ppid==0){
                                                      for(int z=0;z<j;z++){
                    // printf("after pipe:%s\n",pargs[z]);   
                    args=split_line(pargs[z],parse_array);
                    //  printf("after pipe:%s\n",pargs[z]);                
                    // if(dup2(pin,0) != 0){
                        // perror("dup2 failed\n");
                    // }
                    dup2(pin,0);
                    close(pin);

                     if(z == j - 1) pout = dup(stout);
                    else{
                        //int fildes[2];
                        if(pipe(fildes) < 0){
                            perror("pipes failed");
                        }
                        pout = fildes[1];
                        pin = fildes[0];
                    }
                    // if(dup2(pout,1) != 1){
                        // perror("dup2 failed\n");
                    // }
                    dup2(pout,1);
                    close(pout);
                    int r=0;
                    // fore=1;
                    while(args[r]!=NULL){
                        //   printf("%s km",args[r]);
                        if(!strcmp(args[r],"&")) {
                            signal(SIGCHLD,child_handler);
                            fore=0;
                            args[r]=NULL;
                            break;
                        }
                        r++;
                    }
                    // printf("\n");
                    // printf("yes");
                    status=execute(args);
                    free(args);
                }
                dup2(stin,0);
                close(stin);
                 dup2(stout,1);
                close(stout);
                    }
                    else if(ppid>0){
                        printf(" ");
                    }
                }
//------------------------------------------------------------------------------------------------------------------------
                free(pargs);
                
        }
        free(cmdlist);
        free(line);

    
}
    return 0;
}

//--------------------------------------------------------------------------------------------------------------------



void addjob(int pid)
{
    pid_process[num] = pid;
    num++;
    // printf("%d",num);
    return;
}

int printjobs(){
    int status;
    int val=0;
    for(int i=0;i<num;i++){
        // printf("krishna");
        int s=waitpid(pid_process[i],&status,WNOHANG||WUNTRACED);
        if(s>=0){
            val++;
            char pid[10];
            sprintf(pid, "%d", pid_process[i]);
            char dir[60]="/proc/";
            strcat(dir,pid);
            strcat(dir,"/stat");
            char* content=(char*)malloc(sizeof(char)*3000);
            int fd = open(dir, O_RDONLY);
            if(fd == -1){
                // perror("error open file");
                return 1;
            }
            read(fd, content, 1000);
            char** stat1 = split_line(content,parse_array );
            
            char* st;
            st=stat1[2];
            if(!strcmp(st,"S")) st="Running";
            if(!strcmp(st,"T")) st="Stopped";
            printf("[%d]  %s  %s",val,st,stat1[1]);
        } 
    }
    return 1;
}


int killjob(char** args)
{
    int i,val = 0,top = 0, job_id, pid = -1,status;
    char *argv[20];
    while(args[top]!=NULL) top++;
    if(top != 3)
    {
      perror("Invalid usage of the command\n");
      return 1;
    }
    job_id = atoi(args[1]);
    int signal_id = atoi(args[2]);
    for(i = 0 ; i < num ; i++)
    {
        int s=waitpid(pid_process[i],&status,WNOHANG||WUNTRACED);
      if(s == -1)
      {
        continue;
      }
      val++;
      if(val == job_id)
      {
        pid = pid_process[i];
        break;
      }
    }
    if(pid == -1)
    {
      perror("No process with the given job id exists");
      return 1;
    }
    if(kill(pid, signal_id) < 0)
    {
      perror("Invalid usage or wrong signal number");
      return 1;
    }
    return 1;
}
int overkill()
{
  int i,status;
  for(i = 0 ; i < num ; i++)
  {
      int s=waitpid(pid_process[i],&status,WNOHANG||WUNTRACED);
    if(s == -1)
      continue;
      if(kill(pid_process[i],9) < 0)
      {
        perror("No process with the given job id exists");
        return 1;
      }
  }
  return 1;
}
int Fg(char** args)
{
    int top = 0,i;
    fore=1;
    while(args[top]!=NULL) top++;
    if(top != 2)
    {
      perror("Incorrect number of arguments");
      return 1;
    }
    int job_id = atoi(args[1]);
    int val = 0,pid = -1,status;
    for(i = 0 ; i < num ; i++)
    { 
    int s=waitpid(pid_process[i],&status,WNOHANG||WUNTRACED);
      if(s == -1)
      {
        continue;
      }
      val++;
      if(val == job_id)
      {
        pid = pid_process[i];
        break;
      }
    }
    if(pid == -1)
    {
      perror("No process with the given job id exists");
      return 1;
    }
    if(waitpid(pid, &status, WUNTRACED) < 0)
    {
      perror("Not able to execute the command");
      return 1;
    }
    return 1;
}
int Bg(char **args)
{
    int i,val=0,pid=-1,top=0,status;
    while(args[top]!=NULL) top++;
    if(top != 2)
    {
      perror("Incorrect number of arguments");
      return 1;
    }
    int job_id = atoi(args[1]);
    for(i = 0 ; i < num ; i++)
    {
        int s=waitpid(pid_process[i],&status,WNOHANG||WUNTRACED);
      if(s == -1)
      {
        continue;
      }
      val++;
      if(val == job_id)
      {
        pid = pid_process[i];
        break;
      }
    }
    if(pid == -1)
    {
      perror("No process with the given job id exists");
      return 1;
    }
    if(kill(pid, 18) < 0)
    {
      perror("Invalid usage or wrong signal number");
      return 1;
    }
    return 1;
}

int mysetenv(char **args)
{
	int count = 0, i=0;
    // printf("krishna");
	 while(args[count] != NULL)
		count = count + 1;
    if(count <= 1)
			printf("Too few arguments\n" );
	else if(count == 2)
	{
		if(setenv(args[1], "", 1) == -1)
		{
			perror("Shell");
		}			
	}
    else if(count==3)
 	{
		if(setenv(args[1], args[2], 1) == -1)
		{
			perror("Shell");
		}		
	}
    else
		printf("Too many arguments\n");
	
	

	return 1;
}
int myunsetenv(char **args)
{
	int count = 0, i;
	for ( i = 0; args[i] != NULL; ++i)
		count = count + 1;
    if(count <= 1)
			fprintf(stderr,RED "unsetenv: Too few arguments\nUsage: setenv var [value]>\n" RESET);
	else if(count >=3)
			fprintf(stderr,RED "unsetenv: Too many arguments\nUsage: setenv var [value]>\n" RESET);
	
	else
	{
		if(unsetenv(args[1]) == -1)
		{
			perror("Shell");
		}		
	}	
	return 1;
}

void catchZ(int sig)
{

	if(getpid() != mainpid)
		return;
	printf("\n");
	if(childpid != -1)
	{	
		// kill(childpid, SIGTTIN);	
		kill(childpid, SIGTSTP);
        addjob(childpid);
	}
    // signal(SIGINT, catchC);
    childpid=-1;
	 signal(SIGTSTP, catchZ);	
}

void catchC(int sig)
{
	printf("\n");
	if(getpid() != mainpid)
		return;

	if(childpid != -1 )
	{
		kill(childpid, SIGINT);
	}
	 signal(SIGINT, catchC);	

}
