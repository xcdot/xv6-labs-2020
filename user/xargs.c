#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * @brief 调用函数，当前为子进程时则执行程序，父进程return
 * @param 系统程序
 * @param 传参列表
 */

 void run(char *program, char **args) {
    if (fork() == 0) { //创建子进程并执行指定程序，输入参数为args
        exec(program, args);
        exit(0);
    }
    return;
 }
 
 int main(int argc, char *argv[]) {
    char buf[2048]; //初始化输入字符池
    char *buf_p = buf, *buf_p_last = buf; //初始化两个字符指针指向buf

    char *argsbuf[128]; //指针列表：保存参数和输入，最终传给run使用
    char **args = argsbuf;

    //把xargs的参数复制刀argsbuf中
    for (int i=1; i<argc; i++) {
        *args = argv[i];
        args++;
    }

    //用参数指针记录当前参数位置
    char **argsbuf_p = args; 

    //开始读stdin的每个字符，并存在字符池buf中
    while (read(0, buf_p, 1) != 0) {
        //p是buf的指针，read(0,p,1) !=0的意思是：
        //从stdin输入中读取字符（第一个参数为0的意思）
        //每次读取一个字符（第三个字符为1的意思），读取后将字符通过指针p存入到buf数组中
        if (*buf_p==' ' || *buf_p=='\n') {
            *buf_p = '\0'; // 当读到了空格或者换行，标记为空字符
            // 把参数添加到参数缓冲区argsbuf中
            *(argsbuf_p++) = buf_p_last;
            buf_p_last = buf_p+1;

            //每当遇到换行符，则把参数传递给run函数并调用
            if (*buf_p=='\n') {
                *argsbuf_p = 0;
                run(argv[1], argsbuf);
                argsbuf_p = args;
            }
        }
        buf_p++;
    }

    //如果最后一行不是空行，则用相同逻辑继续处理一次
    if (argsbuf_p != args) {
        *buf_p = '\0';
        *(argsbuf_p++) = buf_p_last;
        *argsbuf_p = 0;
        run(argv[1], argsbuf);
    }

    //等待所有子进程结束
    while (wait(0) != -1) {};
    exit(0);
 }