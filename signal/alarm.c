 #include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
  if (signo == SIGALRM) {
    printf("Receives SIGALRM signal in handler.\n");
  }
}


int main(void)
{
  if (signal(SIGALRM, sig_handler) == SIG_ERR) {
    printf("Unable to catch SIGINT\n");
  }

  alarm(15);

  while (1) {
  }

  return 0;
}
