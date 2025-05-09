#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int main(int argc, char *argv[])
{
    char buf = 'P';
    int fd_c2p[2];
    int fd_p2c[2];

    pipe(fd_c2p); //创建c2p管道（子进程->父进程）
    pipe(fd_p2c); //创建p2c管道 (父进程->子进程)

    int pid = fork();
    int exit_status = 0;

    if (pid < 0) {
        printf("fork() error\n");
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);
        exit(1);
    }
    else if (pid == 0) {
        //子进程逻辑
        close(fd_p2c[WR]); //关闭父进程写端
        close(fd_c2p[RD]); //关闭子进程读端

        if (read(fd_p2c[RD], &buf, sizeof(char)) != sizeof (char)) {
            printf("child read() error\n");
            exit_status = 1;
        }
        else {
            printf("%d: received ping\n", getpid());
        }

        if (write(fd_c2p[WR], &buf, sizeof(char)) != sizeof(char)) {
            printf("child write() error\n");
            exit_status = 1;
        }

        close(fd_p2c[RD]);
        close(fd_c2p[WR]);

        exit(exit_status);
    }
    else {
        //父进程逻辑
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);

        if (write(fd_p2c[WR], &buf, sizeof(char)) != sizeof(char)) {
            printf("parent write() error\n");
            exit_status = 1;
        }

        if (read(fd_c2p[RD], &buf, sizeof(char)) != sizeof(char)) {
            printf("parent read() error\n");
            exit_status = 1;
        }
        else {
            printf("%d: received pong\n", getpid());
        }

        close(fd_p2c[WR]);
        close(fd_c2p[RD]);

        exit(exit_status);
    }
}