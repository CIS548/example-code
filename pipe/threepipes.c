#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<assert.h>



int main (int argc, const char * argv[]) {
  int fd[2];
  int fd1[2];

  if (pipe(fd) < 0) perror("pipe error");
  if (pipe(fd1) < 0) perror("pipe error");


  // Create another child process for ls
  int pid0 = fork();
  if (pid0 == 0) {
    int ret = dup2(fd[1],STDOUT_FILENO); // write to standard out
    if (ret < 0) perror("dup2");
    char *argv[10];
    argv[0] = "ls";
    argv[1] = "-l";   
    argv[2] = NULL;

    close(fd[1]);
    close(fd1[1]);

    execvp("ls", argv);
    assert(0);
  }

  // Create one child process for more
  int pid1 = fork();
  if (pid1 == 0) {
    int ret = dup2(fd[0],STDIN_FILENO); // read from standard in
    if (ret < 0) perror("dup2");

    ret = dup2(fd1[1],STDOUT_FILENO); // write to standard out
    if (ret < 0) perror("dup2");
    
    char *argv[10];
    argv[0] = "more";  argv[1] = NULL;

    // close write end of pipes you don't use
    close(fd[1]);
    close(fd1[1]);
    execvp("more", argv);
  }

  // Create one child process for more
  int pid2 = fork();
  if (pid2 == 0) {
    int ret = dup2(fd1[0],STDIN_FILENO); // read stdin via read end of second pipe
    if (ret < 0) perror("dup2");
    char *argv[10];
    argv[0] = "wc";  argv[1] = NULL;

    // close write of pipes you don't use
    close(fd[1]);
    close(fd1[1]);
    execvp("wc", argv);
  } 

  printf("%d %d %d\n", pid0, pid1, pid2);
  // close parent's pipes
  close(fd[1]);
  close(fd1[1]);



  // wait for the more process to finish
  int status;
  waitpid(pid0, &status, 0);
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);

  printf("Done!\n");
  return 0;
}

