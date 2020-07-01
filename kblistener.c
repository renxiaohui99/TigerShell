#include<termios.h>
#include<unistd.h>
#include<stdio.h>
#include<stdbool.h>
bool setListener(bool t){

    if(t){
        struct termios init_attr, new_attr;
        if(tcgetattr(fileno(stdin), &init_attr) == -1){
            printf("func setListener: tcgetattr failed!\n");
            return false;
        }else{

            new_attr = init_attr;
            //切换为非标准输入
            new_attr.c_lflag &= (~ICANON);
            //设置非标准输入超时时间为0，？
            new_attr.c_cc[VTIME] = 0;
            //设置非标准输入最小读取字符数量
            new_attr.c_cc[VMIN] = 1;
            //NL TO CR
            //new_attr.c_iflag &= (INLCR);
            
            new_attr.c_oflag &= (~OPOST);
            //new_attr.c_oflag &= (ONLCR);
            //new_attr.c_lflag &= (ECHOE);
            //new_attr.c_lflag &= (ECHOK);
            //new_attr.c_lflag &= (XCASE);
            //TCSANOW:不等待数据传输完毕就立即改变属性
            if(tcsetattr(0, TCSANOW, &new_attr) == -1){
                printf("func setListener: tcsetattr failed!\n");
                return false;
            }else{
                return true;
            }
        }
    }else{

    }

}
void test(){
    setListener(true);
    char ch;
    while(ch = getchar()){
        printf("%d",ch);
    }
    
}
void test2(){
    struct termios attr; 
    cfmakeraw(&attr);
    tcsetattr(0, TCSANOW, &attr);
    char ch;
    while(ch = getchar()){
        printf("%d",ch);
    }

}
int main(int argc, char** argv){
    test2();
    return 0;
}
