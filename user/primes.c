#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void child(int *p) {
    // read from pipe from parent process
    close(p[1]);
    int buf = 0;
    if (read(p[0], &buf, sizeof(int)) != sizeof(int)) {
        exit(0); // recursive function must have a way out
    }
    int prime = buf;
    printf("prime %d\n", prime);
    
    // initialize pipe to transfer data to child process
    int new_p[2];
    pipe(new_p);

    // fork
    int pid = fork();
    if (pid == 0) {
        child(new_p);
    }
    else {
        // dont't close file descriptor before we call fork, otherwise would cause child process unable to read from pipe
        close(new_p[0]);
        while (read(p[0], &buf, sizeof(int)) == sizeof(int)) {
            if (buf % prime != 0) {
                write(new_p[1], &buf, sizeof(int));
            }
        }
        close(p[0]);
        close(new_p[1]);
        wait(0);
    }
    exit(0);
}

int
main(int argc, char *argv[])
{
  if(argc != 1){
    fprintf(2, "Usage: primes doesn't need arguments.\n");
    exit(1);
  }

  int p[2];
  pipe(p);
  int pid = fork();

  if (pid == 0) {
    child(p);
  } else {
    close(p[0]);

    // write numbers to the pipe
    for (int i = 2; i < 36; i += 1) {
        write(p[1], &i, sizeof(int));
    }
    close(p[1]); // close before wait(0), otherwise would cause the child process to try to read from pipe indefinitely
    
    wait(0);
  }

  exit(0);
}
