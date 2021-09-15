#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("Receives SIGINT signal in handler\n");
  }
}


int main(void)
{
  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    printf("Unable to catch SIGINT\n");
  }

  if (signal(SIGKILL, sig_handler) == SIG_ERR) {
    printf("\ncan't catch SIGKILL\n");
  }

  if (signal(SIGSTOP, sig_handler) == SIG_ERR) {
    printf("\ncan't catch SIGSTOP\n");
  }

  while(1) {
    sleep(1);
  }
  return 0;
}
