/**
 * Brandon Lee
 * CS 444 Kevin McGrath
 * Concurrency 4 Part 1
 * 19 May 2016
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

// Abstract resource representation
struct resource {
    sem_t mutex;
};

// Instantate globals
struct resource theResource;
int processCounter;

// Basic signal catching
void signalCatch(int signal) {
    printf("Catching signal: %d\n", signal);
    sem_destroy(&theResource.mutex);
    kill(0, signal);
    exit(0);
}

// Abstract process representation
void process(void *resource) {

    // Check number of processes
    int needToWait = 0;
    if (processCounter > 0) {
        needToWait = 1;
    }

    // Wait if needed
    while (needToWait == 0) {
        printf("Process is waiting\n");
        sleep(5);

        if (processCounter == 3) {
            printf("Process has finished waiting\n");
            needToWait = 1;
        }
    }

    // Wait and decrement
    sem_wait(&theResource.mutex);
    processCounter = processCounter - 1;

    // Print to user
    printf("Process currently working\n");
    sleep(10);
    printf("Process is has finished working\n");

    // Post and increment
    sem_post(&theResource.mutex);
    processCounter++;
}

int main(int argc, char **argv) {

    // Basic instantiation
    void *processFunction = process;
    struct sigaction signal;
    pthread_t process1, process2, process3, process4, process5, process6;

    // Set signals
    sigemptyset(&signal.sa_mask);
    signal.sa_flags = 0;
    signal.sa_handler = signalCatch;
    sigaction(SIGINT, &signal, NULL);


    // Initialize semaphore to 3
    sem_init(&theResource.mutex, 0, 3);
    processCounter = 3;

    // Create threads
    pthread_create(&process1, NULL, processFunction, NULL);
    pthread_create(&process2, NULL, processFunction, NULL);
    pthread_create(&process3, NULL, processFunction, NULL);
    pthread_create(&process4, NULL, processFunction, NULL);
    pthread_create(&process5, NULL, processFunction, NULL);
    pthread_create(&process6, NULL, processFunction, NULL);

    // Run forever
    while (1) {

    }
}
