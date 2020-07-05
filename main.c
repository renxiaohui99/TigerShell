#define _GNU_SOURCE
#define CLOSE "\001\033[0m\002"
#define BEGIN(x,y) "\001\033["#x";"#y"m\002"

#include "head.h"

#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <readline/readline.h>
#include <signal.h>
void handleSIGINT(){
	//printf("Bye-Bye\n");
	putchar('\n');
	write_history(NULL);
	exit(0);
}
InputLine* malloc_InputLine()
{
	InputLine* input = malloc(sizeof(InputLine));
	if (!input) {
		fprintf(stderr, "memory out\n");
		exit(EXIT_FAILURE);
	}
	input->buffer_pos = 0;
	input->line = malloc(sizeof(char) * MAX_INPUTLINE_SIZE);
	if (!input->line) {
		fprintf(stderr, "memory out\n");
		exit(EXIT_FAILURE);
	}
	return input;
}
void free_InputLine(InputLine* input) {
	if (input != NULL) {
		if (input->line != NULL) {
			free(input->line);
			input->line = NULL;
		}
		free(input);
		input = NULL;
	}
}
void free_cmds(CMD** cmds) {
	if (cmds != NULL) {
		for (size_t i = 0; i < pipeNum + 1; ++i) {
			if (cmds[i] != NULL) {
				if (cmds[i]->argv != NULL) {
					free(cmds[i]->argv);
					cmds[i]->argv = NULL;
				}
				if (cmds[i]->infile != NULL) {
					fclose(cmds[i]->infile);
					cmds[i]->infile = NULL;
				}
				if (cmds[i]->outfile != NULL) {
					fclose(cmds[i]->outfile);
					cmds[i]->outfile = NULL;
				}
				free(cmds[i]);
				cmds[i] = NULL;
			}
		}
		free(cmds);
		cmds = NULL;
	}
}
void read_alias(){

	FILE* af = fopen("./.alias", "r");
	if(af == NULL){
		return ;
	}else{
		char* fc=(char*)malloc(MAX_INPUTLINE_SIZE*sizeof(char));
		char* rc=(char*)malloc(MAX_INPUTLINE_SIZE*sizeof(char));

		while((fscanf(af, "%s", fc))!=EOF){
			fgets(rc, MAX_INPUTLINE_SIZE,af);
			rc[strlen(rc)-1] = '\0';
			//printf("%s: %s\n", fc, rc);
			struct Alias* nali = (struct Alias*)malloc(sizeof(struct Alias));
			nali->fc = (char*)malloc((strlen(fc)+1)*sizeof(char));
			strcpy(nali->fc, fc);
			nali->rc = (char*)malloc((strlen(rc)+1)*sizeof(char));
			strcpy(nali->rc, rc);
			alias_tbl[atx] = nali;
			atx++;
		}
	}

}
void init_shell() {
	read_history(NULL);
	signal(SIGINT, handleSIGINT);
	atx = 0;
	read_alias();
	//extern struct Alias* alias_tbl[kMaxAtx];
}

void init_command() {
	cmds = NULL;
	iptl = NULL;
}


void get_string() {

	char* user = getenv("USER");
	char* hostname = malloc(sizeof(char) * MAX_HOSTNAME_SIZE);
	gethostname(hostname, sizeof(hostname));

	char* dir = get_current_dir_name();
	int dir_len = strlen(dir);
	char* home = getenv("HOME");
	int home_len = strlen(home);
	int pre_len = home_len-strlen(user);
	bool replace;
	if(dir_len<home_len){
		replace = false;
	}else{
		for(int i=0;i<home_len;++i){
			if(dir[i] != home[i]){
				replace = false;
				break;
			}else{
				if(i == home_len - 1){
					replace  = true;
				}
			}
		}
	}
	if(replace){
		int back_len  =dir_len - home_len;
		char* rdir = (char*) malloc((back_len+2)*sizeof(char));
		rdir[0] = '~';
		for(int i=1;i<=back_len+2;++i){
			if(i == back_len+1){
				rdir[i] = '\0';
			}
			rdir[i] = dir[i-1+home_len];
		}
		free(dir);
		dir = rdir;
		rdir = NULL;
	}
	char* user_iden = NULL;
	if(getuid() == 0){
		user_iden = "#";
	}else{
		user_iden = "$";
	}

	char* header = (char*)malloc(MAX_HEADER_SIZE * sizeof(char));
	sprintf(header, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
			BEGIN(33, 40),
			"[",
			CLOSE,
			BEGIN(36, 40),
			user,
			CLOSE,
			BEGIN(33, 40),
			"][",
			CLOSE,
			BEGIN(36, 40),
			hostname,
			CLOSE,
			BEGIN(33, 40),
			"]",
			CLOSE,
			BEGIN(33, 40),
			":",
			CLOSE,
			BEGIN(36, 40),
			dir,
			CLOSE,
			BEGIN(33, 40),
			user_iden,
			CLOSE
				);


	// get string
	iptl = malloc_InputLine();
	iptl->line = readline(header);
	iptl->buffer_pos = strlen(iptl->line);


	if (header != NULL) {
		free(header);
		header = NULL;
	}
	if (hostname != NULL) {
		free(hostname);
		hostname = NULL;
	}
	if (dir != NULL) {
		free(dir);
		dir = NULL;
	}
}

bool check_quit(char *str) {
	if (strcmp(str,"quit") == 0)return true;
	return false;
}
void shell_loop() {
	do {

		init_command();
		get_string();
		set_background();
		delete_space();
		if (strlen(iptl->line)==0) {
			free_InputLine(iptl);
			continue;
		}
		add_history(iptl->line);
		if (check_quit(iptl->line)) {
			free_InputLine(iptl);
			break;
		}
		split();
		if (!fill_cmds()) {
			free_InputLine(iptl);
			free_cmds(cmds);
			continue;
		}
		free_InputLine(iptl);

		if (!execute(cmds)) {
			printf("execute failed.\n");
			free_cmds(cmds);
			continue;
		}
		free_cmds(cmds);
	} while (1);
}


int main(int argc, char* argv[]) {
	init_shell();
	shell_loop();
	write_history(NULL);
	return EXIT_SUCCESS;
}
