#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int n;
    if (argc != 2 || (n = atoi(argv[1])) <= 0) {
        printf("error usage: sleep number\n");
        exit(1);
    }
    sleep(n);
    exit(0);
}