#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

static sigset_t intmask;

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("Receives SIGINT signal in handler\n");
    exit(0);
  }

  if (signo == SIGALRM) {
    if (sigprocmask(SIG_UNBLOCK, &intmask, NULL) == -1) {
      perror("Failed to unblock SIGINT");
    } else {
      printf("SIGINT signal unblocked\n");      
    } 
  }
}


int main(void)
{
  
  if ((sigemptyset(&intmask) == -1) || 
      (sigaddset(&intmask, SIGINT) == -1)) {
    perror("Failed to initialize the signal mask");
    return 1;
  }

  if (signal(SIGINT, sig_handler) == SIG_ERR) {
    perror("Unable to catch SIGINT\n");
  }

  if (signal(SIGALRM, sig_handler) == SIG_ERR) {
    perror("Unable to catch SIGALRM\n");
  }

  if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1) {
    perror("Unable to block SIGINT");
  } else {
    fprintf(stderr, "SIGINT signal blocked\n");
  }

  alarm(15);

  while (1) {
    sleep(1);
  }
  return 0;
}
