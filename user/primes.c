#include "kernel/types.h"
#include "user/user.h"

void
sieve(int read_fd)
{
  int prime, next;
  int buf[280]; // mang tam de chua so da loc
  int count;

  while (1) {
    // doc so dau tien = so nguyen to
    if (read(read_fd, &prime, sizeof(int)) <= 0)
      break;

    printf("prime %d\n", prime);

    // doc tat ca so con lai va loc bo boi cua prime
    count = 0;
    while (read(read_fd, &next, sizeof(int)) > 0) {
      if (next % prime != 0)
        buf[count++] = next;
    }
    close(read_fd);

    if (count == 0)
      break;

    // tao pipe moi, fork child de ghi so da loc vao pipe
    int p[2];
    pipe(p);

    if (fork() == 0) {
      // child: ghi so vao pipe roi thoat
      close(p[0]);
      for (int i = 0; i < count; i++)
        write(p[1], &buf[i], sizeof(int));
      close(p[1]);
      exit(0);
    }

    // parent: doc tu pipe moi o vong lap tiep theo
    close(p[1]);
    read_fd = p[0];
    // khong wait o day, de parent doc pipe khong bi deadlock
  }

  close(read_fd);
  while (wait(0) >= 0); // doi tat ca child ket thuc
  exit(0);
}

int
main()
{
  int p[2];
  pipe(p);

  if (fork() == 0) {
    // child: bat dau sieve
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