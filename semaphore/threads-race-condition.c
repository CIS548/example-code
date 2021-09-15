// EDITED VERSION BELOW DISREGARD PER STUDENT REQUEST //

#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void *print_message(void *ptr);

void main(void) {
    pthread_t thread1, thread2;
    printf("Testing race condition...\n");

    //CREATE THREAD 1
    if (pthread_create(&thread1, NULL, print_message, (void *) "Hello")) {
        perror("thread_create: can't create thread 1");
        exit(EXIT_FAILURE);
    }

    //CREATE THREAD 2
    if (pthread_create(&thread2, NULL, print_message, (void *) "World")) {
        perror("thread_create: can't create thread 2");
        exit(EXIT_FAILURE);
    }
    /*
        Performing a return from the start function of any thread other than
        the main thread results in an implicit call to pthread_exit(), using
        the function's return value as the thread's exit status.

        To allow other threads to continue execution, the main thread should
        terminate by calling pthread_exit() rather than exit(3).
    */
    // exit(EXIT_SUCCESS);
    pthread_exit(EXIT_SUCCESS);

}

void *print_message(void *ptr) {
    // Increase the running time to show race condition.
    sleep(1);
    char *msg = (char *) ptr;
    printf("%s ", msg);
}
