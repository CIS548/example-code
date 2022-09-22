#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
static sigset_t intmask;
//int flag = 0;
void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("Receives SIGINT\n");
    exit(0);
  }
  if (signo == SIGALRM) {
    printf("catch the alarm signal\n");
    //flag = 1;
  }
}
int main(void)
{
  if ((sigemptyset(&intmask) == -1) ||
      (sigaddset(&intmask, SIGINT) == -1)) {
    perror("Failed to initialize the signal mask");
    return 1;
  }
  if (signal(SIGINT, sig_handler) == SIG_ERR)
    perror("Unable to catch SIGINT\n");
  if (signal(SIGALRM, sig_handler) == SIG_ERR)
    perror("Unable to catch SIGALRM\n");
  if (sigprocmask(SIG_BLOCK, &intmask, NULL) == -1) {
    perror("Unable to block SIGINT");
  } else {
    fprintf(stderr, "SIGINT signal blocked\n");
  }
  alarm(10);

  //while (!flag) {}
  printf("Suspending and waiting for alarm\n");
  sigsuspend(&intmask);

  if (sigprocmask(SIG_UNBLOCK, &intmask, NULL) == -1) {
    perror("Failed to unblock SIGINT");
  } else {
    printf("SIGINT signal unblocked\n");
  }

  printf("Suspending for the second time and waiting for ctrl-c\n");
  sigfillset(&intmask);
  sigdelset(&intmask, SIGINT);
  sigsuspend(&intmask);

  while (1) {
    sleep(1);
  }
  return 0;
}
