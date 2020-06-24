#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/wait.h>
const int kMaxCommandSize = 1024;

struct Status{
  char *command;
  int command_size; //不包括终止符
} gs; //global status

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
void Init_command(){
  char* cwd = getcwd(NULL, 0);
  char *user = "user";
  char *machine = "machine";
  printf("\033[1;32m%s>0w0<%s\033[0m", user, machine);
  printf("\033[1;37m:\033[0m");
  printf("\033[1;34m%s",pwd);
  printf("\033[1;37m$ \033[0m");
  delete[] cwd;
  delete[] user;
  delete[] machine
}
void Get_string(){
  gs.command = malloc(kMaxCommandSize * sizeof(char));
  char ch;
  int it = 0;
  while(it<kMaxCommandSize-1 && (ch=getchar())!='\n'){
    gs.command[it++] = ch;
  }
  gs.command[it] = '\0';
  gs.command_size = it;
  return command;
}
int main(int argc, char* argv[]){
    gs.command=NULL;
    while(true){
        //print header
        Init_command();
        //get command
        Get_string();
        //Parse command
        char* c=NULL;
        char** argv=NULL;
        c = malloc(1024*sizeof(char));
        argv = malloc(16*sizeof(char*));
        for(int i=0;i<16;++i){
            argv[i] = malloc(1024*sizeof(char));
        }
        parseCommand(gs.command,c,argv);
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
    delete[] command;
}
