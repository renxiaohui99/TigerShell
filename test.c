#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int main(void)
{
  char* buf = malloc(1024*sizeof(char));
  getcwd(buf, 30);
  printf("%s\n", buf);
  printf("%s\n", getcwd(NULL,0));
  return 0;
}
