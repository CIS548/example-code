#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main (void) {
  // Part 1: Read from a file and print to the terminal -----
  // Open a read-only file descriptor  
  // TODO 1: What is the meaning of O_RDONLY, and why is this flag used here?
  int input_df = open("input_file.txt", O_RDONLY);
  char buf[1024];
  int len = read(input_df, buf, 1024);
  buf[len] = '\0';
  printf("%s\n", buf);
  
  // Part 2: Redirect STDOUT to a new file and write into the file -----
  // Open another file descriptor for writing.
  // TODO 1: What are the meanings of O_WRONLY, O_CREAT, O_TRUNC, and why are these flags used here?
  int new_stdout = open("new_stdout", O_WRONLY | O_CREAT, 0644);
  dup2(new_stdout, STDOUT_FILENO);
  write(STDOUT_FILENO, "Part 2: Hello World!!!", 14);
  close(new_stdout);
  exit (0);
  
  // TODO 2: Compile and run the program
  // gcc redirections.c -o redirections
  // To Learn more
  // https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
}
