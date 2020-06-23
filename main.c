#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/wait.h>
int argj=0;
void parseCommand(char* command, char* c, char* argv[]){
  int index=0;
  while(command[index]!='\0' && command[index]!=' '){
    c[index] = command[index];
    argv[0][index] = command[index];
    index++;
  }
  int argi=1;
  int argj=0;
  //args
  if(command[index] != '\0'){
    while(command[index]!='\0'){
      index++;
      argj=0;
      while(command[index]!='\0' && command[index] != ' '){
        argv[argi][argj] = command[index];
        index++;
        argj++;
      }
      argi++;
    }
  }
  argv[argi]=NULL;

}
int main(int argc, char* argv[]){
  char* command=NULL;
  command = malloc(1024*sizeof(char));
  while(true){
    //print header
    char* pwd = "~/tigerFakePwd";
    printf("\033[1;32musername@tigershell\033[0m");
    printf("\033[1;37m:\033[0m");
    printf("\033[1;34m%s",pwd);
    printf("\033[1;37m$ \033[0m");
    //get command
    char ch;
    int index=0;
    while((ch=getchar())!='\n'){
      command[index++] = ch;
    }
    command[index] = '\0';
    char* c=NULL;
    char** argv=NULL;
    c = malloc(1024*sizeof(char));
    argv = malloc(16*sizeof(char*));
    for(int i=0;i<16;++i){
      argv[i] = malloc(1024*sizeof(char));
    }

    parseCommand(command,c,argv);
    //x command
    pid_t pid = fork();
    if(pid == 0){
      //x process
      execvp(c, argv);
    }else{
      //main process
      int status;
      wait(&status);

    }
  }
  
}
