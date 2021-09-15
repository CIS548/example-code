#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* this program was authored by an anonymous student in CIT595, October 2020 */

sem_t mutex;

void *print_message(void *ptr);

int main(void) {
    pthread_t thread1, thread2;

    /**
     * IDEA: Use a binary semaphore, i.e. mutex,  that makes sure that the second thread is only created once
     * the first one has finished. After we have printed "hello" we also make sure that we wait for the second thread to finish
     * so that we actually see "world" being printed as well.
     *
     * Without this, you have a non deterministic behaviour, the main thread might be closed so fast that neither thread1
     * nor thread 2 have a chance to execute to the end --> nothing printed
     * Or it might be that only "World" or only "Hello" or "World Hello" is printed
     */

    //Initialize mutex, i.e. binary semaphore to 0
    sem_init(&mutex, 0, 0);

    //CREATE THREAD 1
    if (pthread_create(&thread1, NULL, print_message, (void *) "Hello")) {
        perror("thread_create: can't create thread 1");
        exit(EXIT_FAILURE);
    }

    /**
     * In print message, sema_post will change the mutex value to 1. This means print was successful and we can create
     * the second thread.
     * As long as this does not happen, the mutex value is at 1 and we keep on waiting on "sem_wait(&mutex)" until we
     * can enter. At which point we will decrement mutex from 1 to 0 again
     */
    sem_wait(&mutex);

    //CREATE THREAD 2
    if (pthread_create(&thread2, NULL, print_message, (void *) "World")) {
        perror("thread_create: can't create thread 2");
        exit(EXIT_FAILURE);
    }

    //Wait for Thread 2 to complete before exiting Main Thread
    sem_wait(&mutex);

    exit(EXIT_SUCCESS);

}

void *print_message(void *ptr) {

    char *msg = (char *) ptr;
    printf("%s ", msg);

    //Change Mutex to 1, so that we can move on to create second thread
    sem_post(&mutex);

    return NULL;

}