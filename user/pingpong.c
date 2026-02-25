#include "kernel/types.h"
#include "user/user.h"

int
main()
{
  int parent_to_child[2]; // pipe cha -> con
  int child_to_parent[2]; // pipe con -> cha
  char ball = 'x';        // 1 byte bat ky

  pipe(parent_to_child);
  pipe(child_to_parent);

  int pid = fork();

  if (pid == 0) {
    // --- child process ---
    close(parent_to_child[1]);
    close(child_to_parent[0]);

    char received;
    read(parent_to_child[0], &received, 1);
    printf("%d: received ping\n", getpid());

    write(child_to_parent[1], &received, 1);

    close(parent_to_child[0]);
    close(child_to_parent[1]);
    exit(0);

  } else {
    // --- parent process ---
    close(parent_to_child[0]);
    close(child_to_parent[1]);

    write(parent_to_child[1], &ball, 1);

    char received;
    read(child_to_parent[0], &received, 1);
    printf("%d: received pong\n", getpid());

    close(parent_to_child[1]);
    close(child_to_parent[0]);
    wait(0);
    exit(0);
  }
}