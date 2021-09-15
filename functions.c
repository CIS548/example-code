#include <stdlib.h>
#include <stdio.h>

int sum(int varX, int varY) 
{    
  static int funcCounter = 0;
  funcCounter++;
  return (funcCounter + varX + varY);
}

int main (int argc, char** argv)
{
  char *cArray = NULL;
  cArray = 
    (char*) malloc (1024 * sizeof(char));
  int x=0, y = 0;
  x = sum(1,2);
  y = sum(3,4);  
  printf("%d\n", printf("%d %d\n", x, y));
  return 0;
}
