#define _GNU_SOURCE
#define CLOSE "\001\033[0m\002"
#define BEGIN(x,y) "\001\033["#x";"#y"m\002"

#include "head.h"

#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <readline/readline.h>
#include <readline/history.h>

InputLine* malloc_InputLine(InputLine* input, int malloc_type)
{
	switch (malloc_type)
	{
	case MALLOC_INPUTLINE:
		input = malloc(sizeof(InputLine));
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
	default:
		fprintf(stderr, "error malloc type in malloc iptl.\n");
		exit(EXIT_FAILURE);
		break;
	}
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

void init_shell() {
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
		"# ",
		CLOSE
	);

	// get string and history
	read_history(NULL);
	iptl = malloc_InputLine(NULL, MALLOC_INPUTLINE);
	iptl->line = readline(header);
	add_history(iptl->line);
	write_history(NULL);
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


void shell_loop() {
		do {
			init_command();
			get_string();

			if (iptl->buffer_pos == 0) {
				free_InputLine(iptl);
				continue;
			}
			set_background();
			delete_space();
			split();
			if (!fill_cmds()) {
				free_InputLine(iptl);
				free_cmds(cmds);
				continue;
			}
			free_InputLine(iptl);
			if (!execute(cmds)) {
				free_InputLine(iptl);
				free_cmds(cmds);
				continue;
			}
			free_cmds(cmds);
		} while (1);
}


int main(int argc, char* argv[]) {
	init_shell();
	shell_loop();
	return EXIT_SUCCESS;
}
