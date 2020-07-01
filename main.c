#include "head.h"

#include <stdlib.h>
#include <unistd.h>

void print_header()
{
	char* begin = "\033[36;40m";
	char* end = "\033[0m";
	// �û���
	char* user = getenv("USER");
	
	fprintf(stdout, "%s%s%s", begin, user, end);

	//��������
	fprintf(stdout, "\033[33;40m O.O \033[0m");

	// ������
	char* hostname = malloc(sizeof(char) * MAX_HOSTNAME_SIZE);
	gethostname(hostname, sizeof(hostname));
	fprintf(stdout, "%s%s%s:",begin, hostname, end);
	if (hostname != NULL) {
		free(hostname);
		hostname = NULL;
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
InputLine* malloc_InputLine(InputLine* input, int malloc_type)
{
	switch (malloc_type)
	{
		// ���η���
	case MALLOC_INPUTLINE:
		input = malloc(sizeof(InputLine));
		if (!input) {
			fprintf(stderr, "memory out\n");
			exit(EXIT_FAILURE);
		}
		input->buffer_block_cnt = 1;
		input->buffer_pos = 0;
		input->line = malloc(sizeof(char) * INPUT_BUFFER_SIZE * input->buffer_block_cnt);
		if (!input->line) {
			fprintf(stderr, "memory out\n");
			exit(EXIT_FAILURE);
		}
		return input;
		// �ٴη���
	case REALLOC_INPUTLINE:
		if (input == NULL) {
			fprintf(stderr, "null pointer while malloc iptl.\n");
			exit(EXIT_FAILURE);
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
		fprintf(stderr, "error malloc type in malloc iptl.\n");
		exit(EXIT_FAILURE);
		break;
	}
}

void init_shell() {
}

void init_command() {
	print_header();
	cmds = NULL;
	iptl = NULL;
}

InputLine* get_string() {
	// û��ô��ݵ�ʵ��
	InputLine* input = malloc_InputLine(NULL, MALLOC_INPUTLINE);
	// �������int
	int c;
	for (; 1;) {
		c = getchar();
		if (c == EOF || c == '\n') {
			input->line[input->buffer_pos] = '\0';
			return input;
		}
		else {
			input->line[input->buffer_pos] = c;
		}
		++input->buffer_pos;
		// buffer�������Ӵ����ȣ��ٷ���һ��buffer
		if (input->buffer_pos >= INPUT_BUFFER_SIZE * input->buffer_block_cnt) {
			input = malloc_InputLine(input, REALLOC_INPUTLINE);
		}
	}

	// ���ʵ��,�޷��õ�bufsize
	/*
	char* line = NULL;
	size_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
	*/
}

void shell_loop() {
		do {
			init_command();
			iptl = get_string();
			// û������Ͳ�����
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
		        printf("11%s11\n", cmds[0]->argv[0]);
                        printf("11%s11\n", cmds[0]->argv[1]);
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
