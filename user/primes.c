#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_SIZE = sizeof(int);

/**
 * @brief 读取做邻居管道的第一个数据
 * @param lpipe 左邻居管道符
 * @param dst 目标数
 * @return 如果没有数据返回-1，如果有数据返回0
 */
int lpipe_first_int(int lpipe[2], int *dest)
{
    if (read(lpipe[RD], dest, INT_SIZE) == INT_SIZE) {
        printf("prime %d\n", *dest);
        return 0;
    }
    return -1;
}

/**
 * @brief 读取左邻居管道的数据，将不能素数（不能被first整除的数）传到右邻居管道
 * @param lpipe 左邻居管道符
 * @param rpipe 右邻居管道符
 * @param first左邻居的第一个数据
 */
void transmit_data_to_right(int lpipe[2], int rpipe[2], int first) {
    int data;
    while (read(lpipe[RD], &data, INT_SIZE) == INT_SIZE) {
        if (data % first != 0) {
            write(rpipe[WR], &data, INT_SIZE);
        }
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

/**
 * @brief 寻找素数
 * @param lpipe 左邻居管道
 */
void primes(int lpipe[2]) {
    close(lpipe[WR]);
    int first;
    if (lpipe_first_int(lpipe, &first) == 0) { //左管道有读到数
        //创建新管道, 调用transmit函数把读出来的数传到右管道
        int p[2];
        pipe(p);
        transmit_data_to_right(lpipe, p, first);

        if (fork()==0) { //子进程中递归调用
            primes(p);
        }
        else {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);

    for (int i=2; i<=35; i++){
        write(p[WR], &i, INT_SIZE);
    }

    if (fork() == 0) {
        primes(p);
    } 
    else {
        close(p[WR]);
        close(p[RD]);
        wait(0);
    }
    exit(0);
}