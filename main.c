#include "TigerShell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

typedef struct InputLine
{
	// 输入字符串
	char* line;
	// buffer块数目
	size_t buffer_block_cnt;
} InputLine;


void print_header()
{
	char* begin = "\033[36;40m";
	char* end = "\033[0m";
	// 用户名
	char* user = getenv("USER");
	
	fprintf(stdout, "%s%s%s", begin, user, end);

	//花样字体
	fprintf(stdout, "\033[33;40m O.O \033[0m");

	// 主机名
	char* hostname = malloc(sizeof(char) * MAX_HOSTNAME_SIZE);
	gethostname(hostname, sizeof(hostname));
	fprintf(stdout, "%s%s%s:",begin, hostname, end);
	if (hostname != NULL) {
		free(hostname);
		hostname = NULL;
	}
}
int free_InputLine(InputLine* input) {
	if (input != NULL) {
		if (input->line != NULL) {
			free(input->line);
			input->line = NULL;
		}
		free(input);
		input = NULL;
	}
}
InputLine* malloc_InputLine(InputLine* input, int malloc_type)
{
	switch (malloc_type)
	{
	case MALLOC_INPUTLINE:
		free_InputLine(input);
		input = malloc(sizeof(InputLine));
		if (!input) {
			fprintf(stderr, "memory out\n");
			exit(EXIT_FAILURE);
		}
		input->buffer_block_cnt = 1;
		input->line = malloc(sizeof(char) * INPUT_BUFFER_SIZE * input->buffer_block_cnt);
		if (!input->line) {
			fprintf(stderr, "memory out\n");
			exit(EXIT_FAILURE);
		}
		return input;
	case REALLOC_INPUTLINE:
		if (input == NULL) {
			fprintf(stderr, "null pointer while malloc inputline.\n");
			exit(1);
		}
		else {
			++input->buffer_block_cnt;
			input = realloc(input, input->buffer_block_cnt * INPUT_BUFFER_SIZE);
			if (!input->line) {
				fprintf(stderr, "memory out\n");
				exit(EXIT_FAILURE);
			}
		}
		return input;
	default:
		fprintf(stderr, "error malloc type in malloc inputline.\n");
		exit(EXIT_FAILURE);
		break;
	}
}
InputLine* get_string(){
	// 没那么便捷的实现
	InputLine* input = malloc_InputLine(NULL, MALLOC_INPUTLINE);
	size_t buffer_pos = 0;
	// 这里就是int
	int c;
	for (; 1;) {
		c = getchar();
		if (c == EOF || c == '\n') {
			input->line[buffer_pos] = '\0';
			return input;
		}
		else {
			input->line[buffer_pos] = c;
		}
		++buffer_pos;
		if (buffer_pos >= INPUT_BUFFER_SIZE * input->buffer_block_cnt) {
			input = malloc_InputLine(input, REALLOC_INPUTLINE);
		}
	}

	// 便捷实现,无法拿到bufsize
	/*
	char* line = NULL;
	size_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
	*/
}

void shell_loop() {
	while (1)
	{
		InputLine* inputline;
		//int status;
		do {
			print_header();
			inputline = get_string();
			fprintf(stdout, "%s\n", inputline->line);
			free_InputLine(inputline);
			//status = execute(args);
			//free(args);

		} while (1);
	}
}

void init_shell() {
}
void init_command() {

}
int main(int argc, char* argv[]) {
	init_shell();
	init_command();
	shell_loop();
	return EXIT_SUCCESS;
}