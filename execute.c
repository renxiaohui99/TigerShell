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
 *argv: char**, 参数，[0] = 命令, [-1] = NULL
 */
bool cd(CMD* cmd){
	char* to_dir;
	if(cmd->argv[1] == NULL){
		to_dir = getenv("HOME");
	}else if(cmd->argv[2] == NULL){
		to_dir = cmd->argv[1];
	}else if(cmd->argv[3] == NULL){
		to_dir = cmd->argv[2];
	}else{
		printf("cd: too many arguments!\n");
		return false;
	}
	if(chdir(to_dir) == -1){
		printf("cd: chdir failed!\n");
		return false;
	}else{
		return true;
	}
}

int innerCMD(CMD* cmd){
	//0: 内建，执行成功
	//-1： 内建，执行失败
	//1： 不是内建
	int num;
	if(strcmp(cmd->cmd, "cd") == 0){

		if(cd(cmd)){
			num = 0;
		}else{
			num = -1;
		}
	}else{
		num = 1;
	}
	return num;
}

bool do_execute(const char* path,CMD* cmd){
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



		//if(execvp(cmd->cmd, cmd->argv)==-1){
		//    printf("error happens!\n");
		//}
		bool isSuccess;
		if(execv(path, cmd->argv)==-1){
			printf("func do_execute: execv failed!\n");
			isSuccess = false;
		}else{
		
			isSuccess = true;
		}




		//恢复重定向
		if(cmd->infile!=NULL){
			dup2(tmp_in_file, STDIN_FILENO);
		}
		if(cmd->outfile!=NULL){
			dup2(tmp_out_file, STDOUT_FILENO);
		}
		if(isSuccess){
			exit(0);
		}else{
			exit(-1);
		}

	}else{
		int status=0;
		int isSuccess = true;
		if(!cmd->isBackground){
			//waitpid(pid,NULL, 0);
			//if(wait(&status) == -1){
			if(waitpid(pid, NULL, 0) == -1){
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
			int num = innerCMD(cmds[i]);
			if(num == 0){
				//是内建命令，且执行成功
				continue;
			}else if(num == -1){
				//是内建命令，执行失败
				isSuccess = false;
				break;
			}else if(num == 1){
				//不是内建命令
				if(strcmp(cmds[i]->cmd,".") == 0){
					if(-1 == access(cmds[i]->argv[1], X_OK)){
						isSuccess = false;
						break;
					}else if(!do_execute(cmds[i]->argv[1],cmds[i])){
						isSuccess = false;
						break;

					}
				}else{
					const char* cmdpath = NULL;
					if ((cmdpath = searchfile(cmds[i]->cmd)) == NULL) {
						printf("%s can not find.\n", cmds[i]->cmd);
					}
					else if (!do_execute(cmdpath,cmds[i])) {
						isSuccess = false;
						break;
					}
				}
			}
		}
		return isSuccess;
	}

	void test1(){
		printf("===test===\n");
		CMD cmd;
		cmd.cmd = "ls";
		cmd.argv = (char**)malloc(2*sizeof(char*));
		cmd.argv[0] = "ls";
		cmd.argv[1] = "-l";
		cmd.infile = NULL;
		cmd.outfile = fopen("outtestfile.ign", "w+");
		cmd.isBackground = false;
		CMD** cmds = (CMD**)malloc(2*sizeof(CMD*));
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
