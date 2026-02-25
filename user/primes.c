#include "kernel/types.h"
#include "user/user.h"

// Moi lan goi sieve se:
// 1. Doc so dau tien tu pipe -> do la so nguyen to
// 2. Tao pipe moi, fork child de xu ly stage tiep
// 3. Loc cac so con lai, gui sang pipe moi
void
sieve(int read_fd)
{
  int num;

  // doc so dau tien
  if (read(read_fd, &num, sizeof(int)) <= 0) {
    close(read_fd);
    exit(0);
  }

  printf("prime %d\n", num);

  // tao pipe cho stage tiep theo
  int p[2];
  pipe(p);

  int pid = fork();

  if (pid == 0) {
    // child: xu ly stage tiep
    close(p[1]);
    sieve(p[0]);

  } else {
    // parent: loc va chuyen so sang pipe moi
    close(p[0]);

    int next;
    while (read(read_fd, &next, sizeof(int)) > 0) {
      if (next % num != 0) {
        write(p[1], &next, sizeof(int));
      }
    }

    close(read_fd);
    close(p[1]);
    wait(0);
    exit(0);
  }
}

int
main()
{
  int p[2];
  pipe(p);

  int pid = fork();

  if (pid == 0) {
    // child: bat dau pipeline sieve
    close(p[1]);
    sieve(p[0]);

  } else {
    // parent: dua so 2 den 280 vao pipe
    close(p[0]);

    for (int i = 2; i <= 280; i++) {
      write(p[1], &i, sizeof(int));
    }

    close(p[1]);
    wait(0);
    exit(0);
  }
}