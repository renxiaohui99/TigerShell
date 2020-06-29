#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
//const char* kSCommand = {"cd"};
//const int kSCommandSize = sizeof(kSCommand/sizeof(kSCommand[0]));

void test1()
{
}

const int kMaxCommandSize = 256;
const int kMaxHostNameSize = 256;
struct Status
{
    char *command;
    int command_size; //不包括终止符
    char *cwd;
} gs; //global status

void cd(char *to_dir)
{

    if (chdir(to_dir) == 0)
    {
        free(gs.cwd);
        gs.cwd = getcwd(NULL, 0);
    }
}
void parseCommand(char *command, char *c, char *argv[])
{
    int index = 0;
    while (command[index] != '\0' && command[index] != ' ')
    {
        c[index] = command[index];
        argv[0][index] = command[index];
        index++;
    }
    int argi = 1;
    int argj = 0;
    //args
    if (command[index] != '\0')
    {
        while (command[index] != '\0')
        {
            index++;
            argj = 0;
            while (command[index] != '\0' && command[index] != ' ')
            {
                argv[argi][argj] = command[index];
                index++;
                argj++;
            }
            argi++;
        }
    }
    argv[argi] = NULL;
}
void Init_command()
{
    char *cwd = gs.cwd;
    char *user = getenv("USER");
    char *host = malloc(kMaxHostNameSize * sizeof(char));
    gethostname(host, kMaxHostNameSize);
    printf("\033[1;32m%s\033[0m", user);
    printf("\033[1;33m>0w0<\033[0m");
    printf("\033[1;32m%s\033[0m", host);
    printf("\033[1;37m:\033[0m");
    printf("\033[1;34m%s", cwd);
    printf("\033[1;37m$ \033[0m");
    return;
}
void Get_string()
{
    gs.command = malloc(kMaxCommandSize * sizeof(char));
    char ch;
    int it = 0;
    while (it < kMaxCommandSize - 1 && (ch = getchar()) != '\n')
    {
        gs.command[it++] = ch;
    }
    gs.command[it] = '\0';
    gs.command_size = it;
    return;
}
void Init_shell()
{
    gs.command = NULL;
    gs.cwd = getcwd(NULL, 0);
}
void test2() {}
int main(int argc, char *argv[])
{
    Init_shell();
    while (true)
    {
        //print header
        Init_command();
        //get command
        //char* tmp = getenv("PWD");
        Get_string();
        //Parse command
        char *c = NULL;
        char **argv = NULL;
        c = malloc(1024 * sizeof(char));
        argv = malloc(16 * sizeof(char *));
        for (int i = 0; i < 16; ++i)
        {
            argv[i] = malloc(1024 * sizeof(char));
        }
        parseCommand(gs.command, c, argv);
        bool isSCommand = false;
        /*
        for(int i=0;i<kSCommandSize;++i){
            if(strcmp(kSCommand[i], argv[0]) == 0){
                isSCommand = true;
                break;
            }
        }
        */
        if (strcmp("cd", argv[0]) == 0)
        {
            isSCommand = true;
            cd(argv[1]);
        }
        else
        {
        }
        //x command
        if (isSCommand)
        {
        }
        else
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                //x process
                execvp(c, argv);
            }
            else
            {
                //main process
                int status;
                wait(&status);
            }
        }
    }
    free(gs.command);
}
