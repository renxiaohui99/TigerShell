#include "head.h"

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#include <dirent.h>

bool scanfile(char *path, const char *filename)
{
	//	printf("%s中搜索%s\n", path, filename);
	bool res;
	DIR *dir = opendir(path);
	if (dir == NULL)
	{
		res = false;
	}
	else
	{
		res = false;
		struct dirent *entry;
		while (entry = readdir(dir))
		{
			if (0 == strcmp(entry->d_name, filename))
			{
				res = true;
				break;
			}
		}
	}
	return res;
}

const char *searchfile(const char *filename)
{
	const char *kPath = getenv("PATH");
	int size = (strlen(kPath) + 1);
	char *path = (char *)malloc((size) * sizeof(char *));
	//add . to $PATH
	//char* path = malloc((2+size)*sizeof(char*));
	//path[0]='.';
	//path[1]=':';
	for (int i = 0; i < size; ++i)
	{
		//path[i+2] = kPath[i];
		path[i] = kPath[i];
	}
	//strtok具有状态！线程不安全，并且会改变原字符串！
	//printf("=%s\n", filename);
	char *saveptr;
	path = strtok_r(path, ":", &saveptr);
	bool res = false;
	while (path != NULL)
	{
		if (scanfile(path, filename))
		{
			res = true;
			break;
		}
		path = strtok_r(NULL, ":", &saveptr);
	}
	saveptr = NULL;
	free(path);
	path = NULL;
	//
	if (res)
	{
		int path_size = strlen(path);
		int cmd_size = strlen(filename);
		int tot_size = path_size + cmd_size + 2;
		char *filepath = (char *)malloc((tot_size) * sizeof(char));
		for (int i = 0; i < tot_size; ++i)
		{
			if (i == path_size)
			{
				filepath[i] = '/';
			}
			else if (i == tot_size - 1)
			{
				filepath[i] = '\0';
			}
			else if (i < path_size)
			{
				filepath[i] = path[i];
			}
			else if (i > path_size)
			{
				filepath[i] = filename[i - path_size - 1];
			}
		}
		return filepath;
	}
	else
	{
		return NULL;
	}
}
//void test(){
//    searchfile("ls");
//}
//void test2(){
//    searchfile("c");
//}
//void test3(){
//    if(!searchfile("u78")){
//        printf("u78 not found");
//    }
//}
//int main(void){
//    //test();
//    //test2();
//    test3();
//    return 0;
//}
//int main(void){
//    const char* s = searchfile("ls");
//    printf("%s\n", s);
//    return 0;
//}
