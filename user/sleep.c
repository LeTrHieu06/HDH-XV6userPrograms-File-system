#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // kiem tra thieu doi so
  if (argc < 2) {
    fprintf(2, "Usage: sleep <ticks>\n");
    exit(1);
  }

  // doi chuoi sang so bang atoi, goi sleep
  int ticks = atoi(argv[1]);
  sleep(ticks);

  printf("(nothing happens for a little while)\n");
  exit(0);
}