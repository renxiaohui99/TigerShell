#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

/*
 *isBackground: bool, 是否后台运行
 *infile: int, 输入文件号
 *outfile: int， 输出文件号
 */
typedef struct CMDStruct{
    
    FILE* infile;
    FILE* outfile;
    bool isBackground;
    char* cmd;
    char** argv; //[-1] == NULL

} CMD;

bool do_execute(CMD* cmd){
    
    pid_t pid = fork();
    if(pid == 0){
        int tmp_in_file;
        int tmp_out_file;
        //重定向
        if(cmd->infile!=NULL){
            tmp_in_file = dup(STDIN_FILENO);
            //dup2(int oldfd, int newfd), 如果newfd打开，dup2会自动关闭
            dup2(fileno(cmd->infile), STDIN_FILENO);
        }
        if(cmd->outfile!=NULL){
            tmp_out_file = dup(STDOUT_FILENO);
            dup2(fileno(cmd->outfile), STDOUT_FILENO);
        }
        //执行
        execvp(cmd->cmd, cmd->argv);
        //恢复重定向
        if(cmd->infile!=NULL){
            dup2(tmp_in_file, STDIN_FILENO);
        }
        if(cmd->outfile!=NULL){
            dup2(tmp_out_file, STDOUT_FILENO);
        }
    }else{
        int status=0;
        int isSuccess = true;
        if(!cmd->isBackground){
            if(wait(&status) == -1){
                isSuccess = false;
            }
        }
        return isSuccess;
    }

}


/*

 *cmds: CMD* [], [-1]为NULL
 */
bool execute(CMD** cmds){
    int it = -1;
    CMD* nextCMD = NULL;
    int isSuccess = true;
    while((nextCMD = cmds[++it]) != NULL){
        if(!do_execute(nextCMD)){
            isSuccess = false;
            break;
        }
    }
    return isSuccess;

}


int main(void){
    printf("===test===\n");
    CMD cmd;
    cmd.cmd = "ls";
    cmd.argv = malloc(2*sizeof(char*));
    cmd.argv[0] = "ls";
    cmd.argv[1] = "-l";
    cmd.infile = NULL;
    cmd.outfile = fopen("outtestfile.ign", "w+");
    cmd.isBackground = false;
    CMD** cmds = malloc(2*sizeof(CMD*));
    cmds[0] = &cmd;
    cmds[1] =NULL;
    execute(cmds);
    return 0;
    


}
