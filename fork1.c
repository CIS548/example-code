#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  pid_t  pid;
  int status = 0;
  /* fork another process */
  pid = fork();
  if (pid < 0) { /* error occurred */
    fprintf(stderr, "Fork Failed");
    exit(-1);
  }
  else if (pid == 0) { /* child process */
    //execlp("/bin/sleep", "sleep", "100", (char*) NULL);
    //execlp("/bin/ls", "ls", NULL);
    execlp("/usr/bin/grep", "/usr/bin/grep", "asfasfd", "asdfadf", (char*) NULL);
  }
  else { /* parent process */
    /* parent wait for child complete */
    printf("Creating child %d\n", pid);
    pid_t pid1;
    do {
      pid1 = wait (&status);
      if (pid1 == -1) {
	perror("waitpid");
	exit(EXIT_FAILURE);
      }
      if (WIFEXITED(status)) {
	printf("exited, status=%d\n", WEXITSTATUS(status));
      } else if (WIFSIGNALED(status)) {
	printf("killed by signal %d\n", WTERMSIG(status));
      } else if (WIFSTOPPED(status)) {
	printf("stopped by signal %d\n", WSTOPSIG(status));
      } else if (WIFCONTINUED(status)) {
	printf("continued\n");
      }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    
    printf ("Child %d complete with status %d\n", pid1, status);
    exit(0);
  }
}
