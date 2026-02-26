#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define MAX_DEPTH 50

int last[MAX_DEPTH];  

char* get_basename(char *path)
{
  char *p;
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void print_prefix(int level)
{
  for(int i = 0; i < level; i++){
    if(last[i])
      printf("    "); 
    else
      printf("|   "); 
  }
}

void tree(char *path, int level, int maxDepth, int onlyDir)
{
  if(level > maxDepth || level >= MAX_DEPTH)
    return;

  int fd;
  struct stat st;

  if((fd = open(path, 0)) < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(level == 0){
    printf("%s\n", path); 
  } else {
    print_prefix(level - 1);
    if(last[level - 1])
      printf("`-- %s\n", get_basename(path));
    else
      printf("|-- %s\n", get_basename(path));
  }

  if(st.type != T_DIR || level == maxDepth){
    close(fd);
    return;
  }

  struct dirent de;
  int total = 0;

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    char buf[512];
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
      continue;

    strcpy(buf, path);
    char *p = buf + strlen(buf);
    *p++ = '/';
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    struct stat childst;
    if(stat(buf, &childst) < 0)
      continue;

    if(onlyDir && childst.type != T_DIR)
      continue;

    total++;
  }

  close(fd);

  if((fd = open(path, 0)) < 0)
    return;

  int index = 0;

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    char buf[512];
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
      continue;

    strcpy(buf, path);
    char *p = buf + strlen(buf);
    *p++ = '/';
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    struct stat childst;
    if(stat(buf, &childst) < 0)
      continue;

    if(onlyDir && childst.type != T_DIR)
      continue;

    last[level] = (index == total - 1);

    tree(buf, level + 1, maxDepth, onlyDir);
    index++;
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
      maxDepth = atoi(argv[i + 1]);
      i++;
    }
    else{
      path = argv[i];
    }
  }

  tree(path, 0, maxDepth, onlyDir);
  exit(0);
}