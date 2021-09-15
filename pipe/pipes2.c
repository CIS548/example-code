#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>



int main (int argc, const char * argv[]) {
  int fd[2];
  pipe(fd); 

  // Create one child process for more
  int pid = fork();
  if (pid == 0) {
    close(fd[1]); // close write end
    int ret = dup2(fd[0],0); // read from standard in
    //close(fd[0]);
    if (ret < 0) perror("dup2");
    char *argv[10];
    argv[0] = "more";  argv[1] = NULL;
    execvp("more", argv);
  } 
  // Create another child process for ls
  int pid2 = fork();
  if (pid2 == 0) {
    int ret = dup2(fd[1],1); // write to standard out
    close(fd[0]);
    if (ret < 0) perror("dup2");
    char *argv[10];
    argv[0] = "ls";    argv[1] = "-l";   
    argv[2] = NULL;
    execvp("ls", argv);
  }

  // close parent's pipes
  close(fd[0]);
  close(fd[1]);

  // wait for the more process to finish
  int status;
  waitpid(pid, &status, 0);

  printf("Done!\n");
  return 0;
}
