#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAX_DEPTH 50

int last[MAX_DEPTH];

void print_prefix(int level)
{
    for(int i = 0; i < level; i++){
        if(last[i])
            printf("    ");
        else
            printf("│   ");
    }
}

void tree(char *path, int level, int maxDepth, int onlyDir)
{
    if(level > maxDepth)
        return;

    int fd;
    struct stat st;

    if((fd = open(path, 0)) < 0)
        return;

    if(fstat(fd, &st) < 0){
        close(fd);
        return;
    }

    if(level == 0){
        printf("%s\n", path);
    } else {
        print_prefix(level - 1);
        if(last[level - 1])
            printf("└── %s\n", path);
        else
            printf("├── %s\n", path);
    }

    if(st.type != T_DIR){
        close(fd);
        return;
    }

    struct dirent entries[128];
    int count = 0;

    while(read(fd, &entries[count], sizeof(struct dirent)) == sizeof(struct dirent)){
        if(entries[count].inum == 0)
            continue;
        if(strcmp(entries[count].name, ".") == 0 || strcmp(entries[count].name, "..") == 0)
            continue;
        count++;
    }

    close(fd);

    for(int i = 0; i < count; i++){
        char buf[512];
        strcpy(buf, path);
        char *p = buf + strlen(buf);
        *p++ = '/';
        strcpy(p, entries[i].name);

        struct stat childst;
        if(stat(buf, &childst) < 0)
            continue;

        if(onlyDir && childst.type != T_DIR)
            continue;

        last[level] = (i == count - 1);
        tree(buf, level + 1, maxDepth, onlyDir);
    }
}

int main(int argc, char *argv[])
{
    char *path = ".";
    int maxDepth = 1000;
    int onlyDir = 0;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-d") == 0)
            onlyDir = 1;
        else if(strcmp(argv[i], "-L") == 0 && i + 1 < argc){
            maxDepth = atoi(argv[i+1]);
            i++;
        } else
            path = argv[i];
    }

    tree(path, 0, maxDepth, onlyDir);
    exit(0);
}