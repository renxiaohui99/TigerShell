#include "TigerShell.h"

#include <stdio.h>
#include <stdlib.h>



typedef struct InputLine
{
	// 输入字符串
	char* line;
	// buffer块数目
	size_t buffer_block_cnt;
} InputLine;


void print_header()
{

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
InputLine* read_InputLine(void) {

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

char** split_InputLine(InputLine* input) {
	fprintf(stdout, "%s", input->line);
	return NULL;
}

int execute(char** args) {
	return 0;
}

void shell_loop() {
	while (1)
	{
		print_header();
		InputLine* inputline;
		char** args;
		int status;
		do {
			inputline = read_InputLine();
			args = split_InputLine(inputline);
			free_InputLine(inputline);
			status = execute(args);
			//free(args);

		} while (status);
	}
}

void shell_init() {
	printf("Welcome to TigerShell!\n");
}

int main(int argc, char* argv[]) {
	shell_init();
	shell_loop();
	return EXIT_SUCCESS;
}