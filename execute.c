#include "head.h"

#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

/*
 *isBackground: bool, 是否后台运行
 *infile: FILE*, 输入, 对于“|”，由调用程序fopen一个临时buf
 *outfile: FILE*， 输出
 *cmd: char*, 命令
 *argv: char*, 参数，[0] = 命令, [-1] = NULL
 */


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
        //待修改：使用searchfile查询，然后执行
        if(execvp(cmd->cmd, cmd->argv)==-1){
            printf("error happens!\n");
        }
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
    int isSuccess = true;
    for (size_t i = 0; i < pipeNum + 1; ++i) {
		if (!searchfile(cmds[i]->cmd)) {
			printf("%s can not find.\n", cmds[i]->cmd);
		}
		else if (!do_execute(cmds[i])) {
			isSuccess = false;
			break;
		}
    }
    return isSuccess;
}

void test1(){
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

}
void test2(){
    printf("===test2===\n");
    CMD cmd;
    cmd.cmd = "ls";
    cmd.argv = malloc(1*sizeof(char*));
    cmd.argv[0] = "ls";
    cmd.infile = NULL;
    cmd.outfile = fopen("test2_pipeline.ign", "w+");
    cmd.isBackground = false;
    CMD cmd2;
    cmd2.cmd = "cat";
    cmd2.argv = malloc(1*sizeof(char*));
    cmd2.argv[0] = "cat";
    cmd2.infile = cmd.outfile;
    cmd2.outfile = NULL;
    cmd2.isBackground = false;
    CMD** cmds = malloc(3*sizeof(CMD*));
    cmds[0] = &cmd;
    cmds[1] =&cmd2;
    cmds[2] = NULL;
    execute(cmds);
}
//int main(void){
//    //test1();
//    test2();
//    return 0;
//}
