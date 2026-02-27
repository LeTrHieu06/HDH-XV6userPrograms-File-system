#include "kernel/types.h"
#include "user/user.h"

void sieve(int read_fd) __attribute__((noreturn));

void
sieve(int read_fd)
{
  int prime, next;

  // doc so dau tien = so nguyen to
  if (read(read_fd, &prime, sizeof(int)) <= 0) {
    close(read_fd);
    exit(0);
  }

  printf("prime %d\n", prime);

  // tao pipe cho stage tiep theo
  int p[2];
  pipe(p);

  int pid = fork();

  if (pid == 0) {
    // child: xu ly stage tiep
    close(p[1]);
    close(read_fd); // QUAN TRONG: dong FD cua parent, tranh ro ri
    sieve(p[0]);

  } else {
    // parent: loc va chuyen so sang pipe moi
    close(p[0]);

    while (read(read_fd, &next, sizeof(int)) > 0) {
      if (next % prime != 0)
        write(p[1], &next, sizeof(int));
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

  if (fork() == 0) {
    // child: bat dau pipeline sieve
    close(p[1]);
    sieve(p[0]);

  } else {
    // parent: dua so 2 den 280 vao pipe
    close(p[0]);

    for (int i = 2; i <= 280; i++)
      write(p[1], &i, sizeof(int));

    close(p[1]);
    wait(0);
    exit(0);
  }
}