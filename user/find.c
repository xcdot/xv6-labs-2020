#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

/**
 * @brief 查找函数，找到目标文件后返回目标文件的完整路径
 * @param path, 需要查找的文件的初始路径
 * @param target, 需要查找的目标文件名
 */

void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    //先用open函数判断下path的fd返回值是否合法
    if ((fd = open(path, 0)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    //用fstat函数获取到fd的文件类型，传递给st，如果<0则无法stat
    if (fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        return;
    }

    //针对不同的st类型，做不同处理
    switch (st.type) {
        case T_FILE:
            if(strcmp(path+strlen(path)-strlen(target), target) == 0) {
                printf("%s\n", path); //当前path最后的后缀和target相同，直接打印path路径
            }
            break;
        case T_DIR:
            if(strlen(path)+1+DIRSIZ+1 > sizeof(buf)) {
                // path长度 + '/' + '目录名' + 结尾空字符 '\0'，超过buf数组
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(path);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = '\0';
                if(stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                if(strcmp(buf+strlen(buf)-2, "/.") !=0 && strcmp(buf+strlen(buf)-3, "/..") != 0) {
                    find(buf, target);
                }
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        exit(0);
    }
    char target[512];
    target[0] = '/';
    strcpy(target+1, argv[2]);
    find(argv[1], target);
    exit(0);
}