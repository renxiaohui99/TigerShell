#include "head.h"

#include <string.h>
#include <stdlib.h>

const int kMaxCommandSize = 256;
//const int kMaxHostNameSize = 256;


void set_background()
{
    //printf("%d\n", is_background);
    for (int i = 0; i < iptl->buffer_pos; i++)
    {
        if (iptl->line[i] == '&')
        {
            is_background = true;
            break;
        }
    }
    //printf("%d\n", is_background);
    return;
}

void delete_space()
{
    bool isFirstChar = true;
    bool needDelete = false;
    char* afterDelate = (char*)malloc(kMaxCommandSize * sizeof(char));
    int blank = 0;
    int j = 0;
    int deleteNum = 0;
    for (int i = 0; i < iptl->buffer_pos; i++)
    {
        if (iptl->line[i] == ' ')
        {
            blank++;
        }
        else
        {
            blank = 0;
        }

        if (isFirstChar)
        {
            if (blank)
            {
                needDelete = true;
            }
            isFirstChar = false;
        }

        if (blank >= 2)
        {
            needDelete = true;
        }

        if (needDelete)
        {
            deleteNum++;
            needDelete = false;
        }
        else
        {
            afterDelate[j] = iptl->line[i];
            j++;
        }
    }
    iptl->buffer_pos = iptl->buffer_pos - deleteNum;
    if (afterDelate[iptl->buffer_pos - 1] == ' ')
    {
        iptl->buffer_pos--;
    }
    iptl->line = afterDelate;
    //printf("%d\n", iptl->buffer_pos);
    //    for(int i=0;i<iptl->buffer_pos;i++)
    //printf("%c", iptl->line[i]);
    //printf("结束\n");
    //free(afterDelate);
    return;
}

void split()
{
    int num = 0;
    int i, j;
    bool quotation = false;
    for (i = 0, j = 0; i < iptl->buffer_pos; i++)
    {
        if (iptl->line[i] == 34 && quotation == false)  //双引号
        {
            quotation = true;
        }
        else
        {
            if (iptl->line[i] == 34 && quotation == true)
            {
                quotation = false;
            }
        }
        /*printf("%c ", iptl->line[i]);
        printf("%d ", iptl->line[i]);
        printf("%d\n", quotation);*/
        if (iptl->line[i] != ' ')
        {
            w.words[num][j++] = iptl->line[i];
        }
        else 
        {
            if(quotation == true)
                w.words[num][j++] = iptl->line[i];
            else
            {
                w.words[num][j] = '\0';
                ++num;
                j = 0;
            }
        }
    }
    w.words[num][j] = '\0';
    ++num;

    w.wordsNum = num;
    //for (int i = 0; i < w.wordsNum; i++)
    //    printf("%s\n", w.words[i]);
    //printf("w.wordsNum=%d\n", w.wordsNum);
    return;
}

bool fill_cmds()
{
    bool isSuccess = false;
    int pipe[64];
    pipeNum = 0;
    for (int i = 0; i < w.wordsNum; i++)
    {
        if (w.words[i][0] == '|')
        {
            pipe[pipeNum] = i;
            printf("pipe[%d]=%d\n", pipeNum, pipe[pipeNum]);
            pipeNum++;
        }
    }
    //printf("pipeNum=%d\n", pipeNum);

    if (pipeNum > 0) {
        if (pipe[0] == 0)
            return isSuccess;
        if (pipe[pipeNum - 1] == w.wordsNum - 1)
            return isSuccess;
    }
    if (pipeNum > 1)
    {
        for (int j = 0; j < pipeNum-1; j++)
        {
            if (pipe[j + 1] - pipe[j] <= 1)
                return isSuccess;
        }
    }
    cmds = (CMD**)malloc((pipeNum + 1) * sizeof(CMD*));
    if (pipeNum == 0)
    {
        int argvNum = 0;
        CMD* cmd = (CMD*)malloc(sizeof(CMD));
        cmd->cmd = w.words[0];
        cmd->outfile = NULL;
        cmd->infile = NULL;
        for (int j = 0; j < w.wordsNum; j++)
        {
            if (strcmp(w.words[j], ">>") == 0)
            {
                ++j;
                //printf("outfile=%s\n", w.words[j]);
                cmd->outfile= fopen(w.words[j], "w");
            }
            else {
                if (strcmp(w.words[j], "<<") == 0)
                {
                    ++j;
                    //printf("infile=%s\n", w.words[j]);
                    cmd->infile = fopen(w.words[j], "r");
                }
                else {
                    if (strcmp(w.words[j], "&") == 0)
                    {
                        ++j;
                    }
                    else {
                        argvNum++;
                    }
                }    
            }
        }
        printf("argvNum=%d\n", argvNum);
        cmd->argv = (char**)malloc((argvNum+1) * sizeof(char*));
        cmd->argv[argvNum] = NULL;
        for (int j = 0, k = 0; j < w.wordsNum; j++)
        {
            if (strcmp(w.words[j], ">>") != 0 && strcmp(w.words[j], "<<") != 0 && strcmp(w.words[j], "&") != 0)
            {
                cmd->argv[k] = w.words[j];
                printf("argv[%d]=%s\n", k, cmd->argv[k]);
                k++;
            }
            else {
                j++;
            }
        }
        cmd->isBackground = is_background;
        printf("isBackground=%d\n", cmd->isBackground);
        cmds[0] = cmd;
    }

    if (pipeNum >= 1)
    {
        for (int n = 0; n <= pipeNum; n++)
        {
            if (n == 0)
            {
                int argvNum = 0;
                CMD *cmd=(CMD*)malloc(sizeof(CMD));
                cmd->cmd = w.words[0];
                cmd->outfile = NULL;
                cmd->infile = NULL;
                printf("n=%d outfile=buffile\n", n);
                cmd->outfile = fopen("buffile", "w+");
                for (int j = 0; j < pipe[n]; j++)
                {
                    if (strcmp(w.words[j], "<<") == 0)
                    {
                        ++j;
                        printf("n=%d infile=%s\n", n, w.words[j]);
                        cmd->infile = fopen(w.words[j], "r");
                    }
                    else
                    {
                        if (strcmp(w.words[j], "&") == 0)
                        {
                            ++j;
                        }
                        else {
                            argvNum++;
                        }
                    }
                }
                printf("n=%d argvNum=%d\n", n, argvNum);
                cmd->argv = (char**)malloc((argvNum+1) * sizeof(char*));
                cmd->argv[argvNum] = NULL;
                for (int j = 0, k = 0; j < pipe[n]; j++)
                {
                    if (strcmp(w.words[j], ">>") != 0 && strcmp(w.words[j], "<<") != 0 && strcmp(w.words[j], "&") != 0)
                    {
                        cmd->argv[k] = w.words[j];
                        printf("n=%d argv[%d]=%s\n", n, k, cmd->argv[k]);
                        k++;
                    }
                    else {
                        j++;
                    }
                }
                cmd->isBackground = is_background;
                printf("n=%d isBackground=%d\n", n, cmd->isBackground);
                cmds[n] = cmd;
            }
            else {
                if (n == pipeNum)
                {
                    int argvNum = 0;
                    CMD *cmd=(CMD*)malloc(sizeof(CMD));
                    cmd->cmd = w.words[pipe[pipeNum - 1] + 1];
                    cmd->outfile = NULL;
                    cmd->infile = NULL;
                    printf("n=%d infile=buffile\n", n);
                    cmd->infile = fopen("buffile", "r+");
                    for (int j = pipe[pipeNum - 1] + 1; j < w.wordsNum; j++)
                    {
                        if (strcmp(w.words[j], ">>") == 0)
                        {
                            ++j;
                            printf("n=%d outfile=%s\n", n, w.words[j]);
                            cmd->outfile = fopen(w.words[j], "w");
                        }
                        else {
                            if (strcmp(w.words[j], "&") == 0)
                            {
                                ++j;
                            }
                            else {
                                argvNum++;
                            }
                        }  
                    }
                    printf("n=%d argvNum=%d\n", n, argvNum);
                    cmd->argv = (char**)malloc((argvNum +1)* sizeof(char*));
                    cmd->argv[argvNum] = NULL;
                    for (int j = pipe[pipeNum - 1] + 1, k = 0; j < w.wordsNum; j++)
                    {
                        if (strcmp(w.words[j], ">>") != 0 && strcmp(w.words[j], "<<") != 0 && strcmp(w.words[j], "&") != 0)
                        {
                            cmd->argv[k] = w.words[j];
                            printf("n=%d argv[%d]=%s\n", n, k, cmd->argv[k]);
                            k++;
                        }
                        else {
                            j++;
                        }
                    }
                    cmd->isBackground = is_background;
                    printf("n=%d isBackground=%d\n", n, cmd->isBackground);
                    cmds[n] = cmd;
                }
                else {
                    int argvNum = 0;
                    CMD *cmd=(CMD*)malloc(sizeof(CMD));
                    cmd->cmd = w.words[pipe[n - 1] + 1];
                    cmd->outfile = NULL;
                    cmd->infile = NULL;
                    printf("n=%d infile=buffile\n", n);
                    cmd->infile = fopen("buffile", "r+");
                    printf("n=%d outfile=buffile\n", n);
                    cmd->outfile = fopen("buffile", "w+");
                    for (int j = pipe[n - 1] + 1; j < pipe[n]; j++)
                    {
                        argvNum++;
                    }
                    printf("n=%d argvNum=%d\n", n, argvNum);
                    cmd->argv = (char**)malloc((argvNum +1)* sizeof(char*));
                    cmd->argv[argvNum] = NULL;
                    for (int j = pipe[n - 1] + 1, k = 0; j < pipe[n]; j++)
                    {
                        if (strcmp(w.words[j], ">>") != 0 && strcmp(w.words[j], "<<") != 0 && strcmp(w.words[j], "&") != 0)
                        {
                            cmd->argv[k] = w.words[j];
                            printf("n=%d argv[%d]=%s\n", n, k, cmd->argv[k]);
                            k++;
                        }
                        else {
                            if (strcmp(w.words[j], "&") == 0)
                            {
                                ++j;
                            }
                            else {
                                j++;
                            }
                        }
                    }
                    cmd->isBackground = is_background;
                    printf("n=%d isBackground=%d\n", n, cmd->isBackground);
                    cmds[n] = cmd;
                }
            }
        } 
    }
    isSuccess = true;
    return isSuccess;
}


//int main()
//{
//    iptl->line = (char*)malloc(kMaxCommandSize * sizeof(char));
//    //iptl->line = "  ls    -s -a &  \0";
//    //iptl->buffer_pos = strlen("  ls    -s -a &  \0");
//    //iptl->line = "ls -l";
//    //iptl->buffer_pos = strlen("ls -l");
//    /*iptl->line = "cat \"hello word\"";
//    iptl->buffer_pos = strlen("cat \"hello word\"");*/
//    //iptl->line = "ls -a >> outfile.txt -s";
//    //iptl->buffer_pos = strlen("ls -a >> outfile.txt -s");
//    iptl->line = "ls -l << infile.txt | grep \".c\" | cat >> outfile.txt &";
//    iptl->buffer_pos = strlen("ls -l << infile.txt | grep \".c\" | cat >> outfile.txt &");
//    //printf("%d\n", iptl->buffer_pos);
//    set_background();
//    delete_space();
//    split();
//    printf("fill_cmds isSuccess=%d\n",fill_cmds());
//    return 0;
//}