#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int main(void)
{
    FILE* f= fopen("main.c", "r");
    printf("%d\n", fileno(f));
    char* buf = malloc(1024*sizeof(char));
    getcwd(buf, 30);
    printf("%s\n", buf);
    printf("%s\n", getcwd(NULL,0));
    char* cwd = getenv("PWD");
    printf("%s\n", cwd);
    return 0;
}
