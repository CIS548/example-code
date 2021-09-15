#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

int main (void) {
  int n;
  int fd[2];
  pid_t pid;
  char  line[1024];
  
  if (pipe(fd) < 0) perror("pipe error");

  if ((pid = fork()) < 0) {
    perror("fork error");
  } 
  else if (pid > 0) { /* parent */
    close(fd[0]);  
    write(fd[1], "Sending data over the pipe!\n", 28);
    wait(NULL);
    exit(0);
  } else { /** child */
    close (fd[1]); 
    n = read(fd[0], line, 1024);
    write(STDOUT_FILENO, line, n);
    exit(0);
  }
  exit (0);
}
