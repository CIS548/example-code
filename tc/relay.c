#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define SLEEP_TIME 5

int child_pid = 0;

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("Relaying SIGINT to child\n");
    kill(child_pid, SIGINT);
  }
}


void child_sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("Child receives SIGINT\n");
  }
}


int main()
{

  child_pid = fork();
  if (child_pid) {
    // Parent
    // Isolate child in a process group with setpgid()
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
      printf("Unable to catch SIGINT\n");
    }
    
    setpgid(child_pid, child_pid);
    // Give child terminal control
    sleep(SLEEP_TIME);
    while(1);
  } else {
    // Child
    if (signal(SIGINT, child_sig_handler) == SIG_ERR) {
      printf("Unable to catch SIGINT\n");
    }

    while(1);
  }
  return 0;
}
