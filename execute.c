#include "head.h"

#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include <time.h>
void store_alias(){
	FILE* af = fopen("./.alias", "w");
	if(af!=NULL){
		for(int i=0;i<atx;++i){
			fprintf(af, "%s %s\n", alias_tbl[i]->fc, alias_tbl[i]->rc);
		}
	}
	fclose(af);
	af = NULL;
}
void remove_alias(char* fc){
	//int pos=-1;
	for(int i=0;i<atx;++i){
		if(strcmp(alias_tbl[i]->fc, fc) == 0){
			for(int j=i;j<atx;++j){
				if(j == atx-1){
					alias_tbl[j] = NULL;
				}else{
					alias_tbl[j] = alias_tbl[j+1];
				}
			}
			--atx;
			store_alias();
		}
	}
}
/*
   void e_tmp(){


   }
 */
bool addAlias(char* fc, char* rc){
	//		extern int atx;
	//	extern struct Alias* alias_tbl[kMaxAtx];
	if(strlen(rc) == 0){
		return false;
	}
	for(int i=0;i<atx;++i){
	
		if(strcmp(alias_tbl[i]->fc, fc)==0){
		
			free(alias_tbl[i]->rc);
			alias_tbl[i]->rc = (char* )malloc((strlen(rc)+1)*sizeof(char));
			strcpy(alias_tbl[i]->rc, rc);
			store_alias();
			return true;

		}
	}
	if(atx<kMaxAtx){

		struct Alias* alias_new = (struct Alias*)malloc(sizeof(struct Alias));
		alias_new->fc = (char* )malloc((strlen(fc)+1)*sizeof(char));
		strcpy(alias_new->fc, fc);
		alias_new->rc = (char* )malloc((strlen(rc)+1)*sizeof(char));
		strcpy(alias_new->rc, rc);
		//bp2

		//保存现在的cmds
		//解析tc指向的字符串
		//执行tc解析出来的命令
		//还原cmds




		alias_tbl[atx] = alias_new;
		++atx;
		store_alias();
		//printf("bp4\n");
		return true;
	}else{
		printf("too many alias!\n");
		return false;
	}

}

bool alias(CMD* cmd){
	if(cmd->argv[1] == NULL){
		for(int i=0;i<atx;++i){
			printf("%s:%s\n", alias_tbl[i]->fc, alias_tbl[i]->rc);
		}
		return true;
	}
	
	char* fc = NULL; //fake cmd
	char* rc = (char*)malloc(MAX_INPUTLINE_SIZE*sizeof(char)); //real cmd
	int ax = 0;
	while(cmd->argv[1][ax] != '=' && cmd->argv[1][ax] != '\0'){
		++ax;
	}
	if(cmd->argv[1][ax] == '\0'){
		printf("alias: alias <name>=\"<cmd>\".\n");
		return false;
	}else{
		cmd->argv[1][ax] = '\0';
		fc = &cmd->argv[1][0];
		strcpy(rc, &cmd->argv[1][ax+1]);
		//rc = &cmd->argv[1][ax+1];
		if(rc[0]!='\"'){
			printf("alias: alias <name>=\"<cmd>\".\n");

		}else{
			if(rc[strlen(rc)-1]!='\"'){

				printf("alias: alias <name>=\"<cmd>\".\n");
			}else{
				//bp1
				rc[strlen(rc)-1] = '\0';
				

				if(addAlias(fc, rc+1)){
					return true;
				}else{
					return false;
				}
			}
		}
	}
	return false;
}

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
		printf("cd: chdir failed, check if path is existed!\n");
		return false;
	}else{
		return true;
	}
}

bool history(CMD* cmd) {
	if (cmd->argv[1] == NULL) {
		HIST_ENTRY** his = history_list();
		if (his != NULL) {
			for (size_t i = 0; his[i] != NULL; ++i) {
				time_t time_stamp = history_get_time(his[i]);
				struct tm* pt = localtime(&time_stamp);
				fprintf(stdout,  "%d-%02d-%02d %02d:%02d:%02d \033[40;33m%s\033[0m\n",
						pt->tm_year + 1900,
						pt->tm_mon + 1,
						pt->tm_mday,
						pt->tm_hour,
						pt->tm_min,
						pt->tm_sec,
						his[i]->line);
			}
		}
		else {
			fprintf(stdout, "history: no history records.\n");
			return false;
		}
	}else if (cmd->argv[2] == NULL) {
		for (size_t j = 0; j < strlen(cmd->argv[1]); ++j) {
			if (cmd->argv[1][j] < '0' || cmd->argv[1][j]>'9') {
				fprintf(stderr, "history: unexpected arguments!\n");
				return false;
			}
		}
		HIST_ENTRY** his = history_list();
		if (his!=NULL) {
			size_t i = 0;
			for (; his[i] != NULL; ++i);
			long int dest_pos = i - atoi(cmd->argv[1]);
			dest_pos = dest_pos > 0 ? dest_pos : 0;
			for (i = dest_pos; his[i] != NULL; ++i) {
				time_t time_stamp = history_get_time(his[i]);
				struct tm* pt = localtime(&time_stamp);
				fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d \033[40;33m%s\033[0m\n",
						pt->tm_year + 1900,
						pt->tm_mon + 1,
						pt->tm_mday,
						pt->tm_hour,
						pt->tm_min,
						pt->tm_sec,
						his[i]->line);
			}
		}
		else {
			fprintf(stdout, "history: no history records.\n");
			return false;
		}
	}else {
		fprintf(stderr, "history: too many arguments!\n");
		return false;
	}
	return true;
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
	}else if (strcmp(cmd->cmd,"history")==0) {
		if (history(cmd)) {
			num = 0;
		}else {
			num = -1;
		}
	}else if(strcmp(cmd->cmd, "alias") == 0){
		if(alias(cmd)){
			num = 0;
		}else{
			num = -1;
		}
	}else if(strcmp(cmd->cmd, "unalias") == 0){
		if(cmd->argv[1] == NULL){
			printf("unalias: unalias alias\n");
			
		}else{
			remove_alias(cmd->argv[1]);
			
		}
		num = 0;
	}else{
		num = 1;
		//查询是否是alias命令
		for(int i=0;i<atx;++i){
			if(strcmp(cmd->cmd, alias_tbl[i]->fc)==0){
				num = 2;
			
			}	
		}

	}
	return num;

}
void cleanCMD(CMD* cmd){
	char* arg = NULL;
	int ax = 1;
	while((arg = cmd->argv[ax]) != NULL){
		if(arg[0] == '$'){
			char* rv = getenv(&arg[1]);
			if(rv != NULL){
				strcpy(cmd->argv[ax], rv);
			}
		}
		++ax;
	}
}
bool alias_execute(char* fc){
	for(int i=0;i<atx;++i){
		if(strcmp(alias_tbl[i]->fc, fc) == 0){
		
			char* ttmp = (char*) malloc((1+strlen(alias_tbl[i]->rc))*sizeof(char));
			if(ttmp == NULL){
				return false;
			}
			strcpy(ttmp, alias_tbl[i]->rc);
		

			iptl = malloc_InputLine();
			iptl->line = ttmp;
		
			iptl->buffer_pos = strlen(iptl->line);

			set_background();
			delete_space();
			CMD** alias_cmds;
			if (strlen(iptl->line)==0) {
			
				return true;	
				//free_InputLine(iptl);
				//alias_cmds = NULL;
			}else{
				split();
				CMD** cmds_tmp = cmds;
				if (!fill_cmds()) {
					free_InputLine(iptl);
					free_cmds(cmds);
					cmds = cmds_tmp;
					return false;
				}else{

						
					free_InputLine(iptl);
					if (!execute(cmds)) {
						printf("execute failed.\n");
						free_cmds(cmds);
						cmds=cmds_tmp;
						return false;
					}else{
						//alias_new->rc = cmds;
						//store_alias();
						//free_cmds(cmds);
						cmds=cmds_tmp;
						return true;
					}
				}
			}
			//return true;
			//return execute(alias_tbl[i]->rc);
		}
	}
	return false;
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
		}else{
			printf("pid: %d\n", pid);
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
			cleanCMD(cmds[i]);
			int num = innerCMD(cmds[i]);
			if(num == 0){
				//是内建命令，且执行成功
				continue;
			}else if(num == -1){
				//是内建命令，执行失败
				isSuccess = false;
				break;
			}else if(num ==2){
				if(alias_execute(cmds[i]->cmd)){
					isSuccess = true;
				}else{
					isSuccess = false;
				}


			}else if(num == 1){
				//printf("命令类型判断正确\n");
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
