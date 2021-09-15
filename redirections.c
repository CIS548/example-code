#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main (void) {
  int new_stdout = open("new_stdout", O_WRONLY | O_CREAT, 0644);
  dup2(new_stdout, STDOUT_FILENO);
  write(STDOUT_FILENO, "Hello World!!!", 14);
  close(new_stdout);
  exit (0);
}
