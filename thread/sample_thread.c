// The main thread is supposed to create 10 threads (T0, T1, .., T9).
// Each thread should increase a local variable i (whose initial value is randomly generated) by one, followed by a random sleep time. 


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *mythread(void *arg);

int main(void) {
  pthread_t tids[10];
  int i;
  int id[10];

  for (i=0; i<10; i++) id[i] = i;

  srand(0);

  // Spawn threads, and pass a random value to each thread
  for (i=0; i<10; i++) {
    pthread_create(&tids[i], NULL, mythread, (void*) (id+i));
  }

  // Make sure the main thread terminates only after all spawned threads terminate
  // Also pthread_join enable the caller get the return value from the callees
  for (i=0; i<10; i++)
    pthread_join(tids[i], NULL);

  return 0;
}

// Describe the behavior of a thread - main part
void *mythread(void *arg) {

  int id = *((int*) arg);
  int i;

  int value = rand()%10;

  // Increase local variable by one, and print the value on the screen
  for (i=0; i<3; i++) {
    printf("Thread ID %d: I is now %d.\n", id, i+value);
    sleep(value);
  }

  // Exit - the current thread terminates
  pthread_exit(NULL);
}
