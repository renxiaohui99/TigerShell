#ifndef TIGER_SHELL_HEADER
#define TIGER_SHELL_HEADER

#include <stdio.h>
#include<stdbool.h>

#define MALLOC_INPUTLINE 0
#define MAX_INPUTLINE_SIZE 2048

#define MAX_HOSTNAME_SIZE 512
#define MAX_HEADER_SIZE 1024

#define maxWordNum 256
#define maxWordSize 64

typedef struct CMDStruct {

	FILE* infile;
	FILE* outfile;
	bool isBackground;
	char* cmd;
	char** argv; //[-1] = NULL

} CMD;

typedef struct InputLine
{
	// 输入字符串
	char* line;
	// 字符串最后一位(也就是'\0')的下标,也是所有不包括换行的输入串长度
	unsigned long long buffer_pos;
} InputLine;

typedef struct Word
{
	char words[maxWordNum][maxWordSize];
	int wordsNum;
} Word;

void init_shell();
void init_command();
void get_string();

void set_background();
void delete_space();
void split();
bool fill_cmds();

bool do_execute(const char* path, CMD *cmd);
bool execute(CMD** cmds);

const char* searchfile(const char* filename);
bool scanfile(char* path, const char* filename);

Word w;
CMD** cmds;
InputLine *iptl;
bool is_background; //后台运行标识符
int pipeNum;

#endif
