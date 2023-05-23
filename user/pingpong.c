#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 1){
    fprintf(2, "Usage: pingpong doesn't need arguments.\n");
    exit(1);
  }

  int p[2];
  pipe(p);

  int pid = fork();
  // use read and write system call to ensure the print order of parent and child process
  if (pid == 0) {
    // child process
    char buf[8];
    read(p[0], buf, 1);
    printf("%d: received ping\n", getpid());
    write(p[1], "a", 1);
    close(p[0]);
    close(p[1]);
  } else {
    // parent process
    char buf[8];
    write(p[1], "a", 1);
    read(p[0], buf, 1);
    printf("%d: received pong\n", getpid());
    close(p[0]);
    close(p[1]);
  }

  exit(0);
}
