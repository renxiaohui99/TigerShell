#ifndef TIGER_SHELL_HEADER
#define TIGER_SHELL_HEADER

#include <stdio.h>
#include<stdbool.h>
// main�������õ�
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
	// �����ַ���
	char* line;
	// buffer����Ŀ
	size_t buffer_block_cnt;
	// �ַ������һλ(Ҳ����'\0')���±�,Ҳ�����в��������е����봮����
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
bool is_background; //��̨���б�ʶ��
int pipeNum;

#endif
