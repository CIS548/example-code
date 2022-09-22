#include <unistd.h>
#include <signal.h>
#include <stdio.h>

#define SLEEP_TIME 15

int main()
{
  int child_pid = fork();
  if (child_pid) {
    // Parent
    // Isolate child in a process group with setpgid()
    setpgid(child_pid, child_pid);
    // Give child terminal control
    printf("Parent: I'm going to give my child TC and go to sleep for a bit.\n");
    tcsetpgrp(STDIN_FILENO, child_pid);
    // Take a quick nap
    sleep(SLEEP_TIME);
    /*
     * Parent is in background, so if it were to call tcsetpgrp() it would receive a signal
     * from UNIX. However, it can just block the signal
     */
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(STDIN_FILENO, getpgid(0));
    printf("Parent: I woke up and took back terminal control.\n");
    while(1);
  } else {
    // Child
    while(1);
  }
  return 0;
}
