#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXLINE 1024

int
readline(int fd, char *buf)
{
  int i = 0;
  char c;

  while(i < MAXLINE-1){
    int n = read(fd, &c, 1);
    if(n < 1)
      break;
    buf[i++] = c;
    if(c == '\n')
      break;
  }

  buf[i] = 0;
  return i > 0;
}

int
main(int argc, char *argv[])
{
  int quiet = 0;
  char *file1;
  char *file2;

  if(argc == 4 && strcmp(argv[1], "-q") == 0){
    quiet = 1;
    file1 = argv[2];
    file2 = argv[3];
  }
  else if(argc == 3){
    file1 = argv[1];
    file2 = argv[2];
  }
  else{
    printf("usage: diff [-q] f1 f2\n");
    exit(1);
  }

  int fd1 = open(file1, 0);
  int fd2 = open(file2, 0);

  if(fd1 < 0 || fd2 < 0){
    printf("diff: cannot open file\n");
    exit(1);
  }

  char line1[MAXLINE], line2[MAXLINE];
  int lineno = 1;
  int different = 0;

  while(1){
    int r1 = readline(fd1, line1);
    int r2 = readline(fd2, line2);

    if(!r1 && !r2)
      break;

    if(r1 && r2){
      if(strcmp(line1, line2) != 0)
        different = 1;
    }
    else{
      different = 1;
    }

    if(different){
      if(quiet){
        printf("files differ\n");
        close(fd1);
        close(fd2);
        exit(1);
      }

      if(r1 && r2){
        printf("%s:%d: < %s", file1, lineno, line1);
        printf("%s:%d: > %s", file2, lineno, line2);
      }
      else if(!r1 && r2){
        printf("%s:%d: < EOF\n", file1, lineno);
        printf("%s:%d: > %s", file2, lineno, line2);
      }
      else if(r1 && !r2){
        printf("%s:%d: < %s", file1, lineno, line1);
        printf("%s:%d: > EOF\n", file2, lineno);
      }
    }

    lineno++;
  }

  close(fd1);
  close(fd2);

  if(quiet && !different)
    exit(0);

  exit(different);
}