#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
//#include <string.h>

#define INPUT_SIZE 100

char* getCommandFromInput();
void getUserInput();

int main(int argc, char **argv) {
  getUserInput();
  return 0;
}

char* getCommandFromInput() {
  char *inputBuffer = (char *) malloc(INPUT_SIZE);
  //char inputBuffer[INPUT_SIZE];
  int numBytes = read(STDIN_FILENO, inputBuffer, INPUT_SIZE);  
  printf("Number of bytes read: %d\n", numBytes);
  write(STDOUT_FILENO, "Program is done!!\n", 18);
  return inputBuffer;
}

void getUserInput() {
  char command[INPUT_SIZE];
  char* inputBuffer = getCommandFromInput();

  int i = 0;
  for (i=0; i < INPUT_SIZE; i++) {
    command[i] = inputBuffer[i];
    if (command[i] == '\n') {
      break;
    }
  }
  write(STDOUT_FILENO, command, i+1);
  free(inputBuffer);
}






