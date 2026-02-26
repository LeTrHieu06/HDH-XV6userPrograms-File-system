#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

#define MAXPATH 512

int flag_a = 0;
int flag_s = 0;

long
du(char *path)
{
  int fd;
  struct stat st;
  long total = 0;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "du: cannot open %s\n", path);
    return 0;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "du: cannot stat %s\n", path);
    close(fd);
    return 0;
  }

  if(st.type == T_FILE){
    total = st.size;
    if(flag_a && !flag_s){
      printf("%ld %s\n", total, path);
    }
    close(fd);
    return total;
  }

  if(st.type == T_DIR){
    struct dirent de;
    char buf[MAXPATH];
    int len;

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)){
      printf("du: path too long\n");
      close(fd);
      return 0;
    }

    strcpy(buf, path);
    len = strlen(buf);
    buf[len++] = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;

      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      memmove(buf + len, de.name, DIRSIZ);
      buf[len + DIRSIZ] = 0;

      total += du(buf);
    }

    close(fd);

    if(!flag_s){
      if(flag_a || !flag_a){
        printf("%ld %s\n", total, path);
      }
    }

    return total;
  }

  close(fd);
  return 0;
}

int
main(int argc, char *argv[])
{
  char *path = ".";
  int i;

  for(i = 1; i < argc; i++){
    if(strcmp(argv[i], "-a") == 0)
      flag_a = 1;
    else if(strcmp(argv[i], "-s") == 0)
      flag_s = 1;
    else
      path = argv[i];
  }

  long total = du(path);

  if(flag_s){
    printf("%ld %s\n", total, path);
  }

  exit(0);
}