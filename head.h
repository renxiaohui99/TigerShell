#ifndef TIGER_SHELL_HEADER
#define TIGER_SHELL_HEADER

#include <stdio.h>
#include<stdbool.h>
// main函数内用到
#define MALLOC_INPUTLINE 0
#define REALLOC_INPUTLINE 1

#define INPUT_BUFFER_SIZE 128
#define MAX_HOSTNAME_SIZE 128

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
	// buffer块数目
	size_t buffer_block_cnt;
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
InputLine* get_string();

void set_background();
void delete_space();
void split();
bool fill_cmds();

bool do_execute(CMD *cmd);
bool execute(CMD** cmds);

bool searchfile(char* filename);
bool scanfile(char* path, char* filename);

Word w;
CMD** cmds;
InputLine *iptl;
bool is_background; //后台运行标识符
int pipeNum;

#endif
