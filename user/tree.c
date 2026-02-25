#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void print_indent(int level)
{
    for(int i = 0; i < level; i++)
        printf("│   ");
}

void tree(char *path, int level, int maxDepth, int onlyDir)
{
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;

    if(level > maxDepth)
        return;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "tree: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        close(fd);
        return;
    }

    print_indent(level);
    printf("%s\n", path);

    if(st.type != T_DIR){
        close(fd);
        return;
    }

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;

        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        struct stat childst;
        if(stat(buf, &childst) < 0)
            continue;

        if(onlyDir && childst.type != T_DIR)
            continue;

        tree(buf, level + 1, maxDepth, onlyDir);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    char *path = ".";
    int maxDepth = 1000;   
    int onlyDir = 0;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-d") == 0){
            onlyDir = 1;
        }
        else if(strcmp(argv[i], "-L") == 0 && i + 1 < argc){
            maxDepth = atoi(argv[i+1]);
            i++;
        }
        else{
            path = argv[i];
        }
    }

    tree(path, 0, maxDepth, onlyDir);
    exit(0);
}