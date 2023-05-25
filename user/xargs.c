#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);

  int pid = fork();
  if (pid == 0) {
    // make file descriptor 0 to refer to the object that the read end of the pipe refers to
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);

    // construct new_argv, ignoring the first element in argv, which is "xargs"
    char *new_argv[MAXARG];
    int i;
    for (i = 1; i < argc; i++) {
        new_argv[i-1] = argv[i];
    }

    // read from pipe
    char buf[512], *index;
    index = buf;
    while (read(0, index, 1) == 1) {
        if (*index == '\n') {
            // copy string to tmp
            *index = 0;
            char *tmp = malloc(strlen(buf) * sizeof(char));
            strcpy(tmp, buf);

            // append tmp to new_argv
            new_argv[i-1] = tmp;
            i++;

            // move index to the beginning
            index = buf;
        } else {
            index++;
        }
    }
    exec(argv[1], new_argv);
  } else {
    // parent process, read character from standard input and feed it to the write end of the pipe
    char buf;
    while (read(0, &buf, 1) == 1) {
        write(p[1], &buf, 1);
    }
    close(p[0]);
    close(p[1]);
    wait(0); // still, remember to close before wait
  }

  exit(0);
}
