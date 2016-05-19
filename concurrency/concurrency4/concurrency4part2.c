/**
 * Brandon Lee
 * CS 444 Kevin McGrath
 * Concurrency 4 Part 2
 * 19 May 2016
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

// Abstract chair representation
struct chair {
    struct chair *next;
};

// Abstract queue representation
struct queue {
    struct chair *current;
    struct chair *next;
    int chairs;
    int customerCount;
};

// Global declarations
struct queue theQueue;
pthread_mutex_t barberLock;
pthread_mutex_t customerLock;

// Basic signal catching
void signalCatch(int sig) {
    printf("Catching signal: %d\n", sig);
    kill(0, sig);
    exit(0);
}

// Move chair to queue
void queue_push(void) {
    struct chair new;
    struct chair *ref = theQueue.current;

    // Check if queue has space
    if (theQueue.customerCount >= theQueue.chairs) {
        return;
    }

    new.next = NULL;

    // Link up chairs while links still present
    while (theQueue.next != NULL) {
        ref = ref->next;
    }

    ref->next = &new;
    theQueue.customerCount++;
}

// Remove chair from queue
void queue_pop(void) {
    struct chair *ref = theQueue.current;

    // Decrement and link
    theQueue.customerCount--;
    theQueue.current = theQueue.next;

    // Link up
    if (theQueue.next != NULL) {
        theQueue.next = theQueue.current->next;
    }

    ref = NULL;
}

// Cut hair representation
void cut_hair(void) {
    printf("Barber: Cutting hair!\n");
    sleep(10);
    printf("Barber: Finished cutting hair!\n");
}

// Getting haircut representation
void get_hair_cut(void) {
    printf("Customer: Getting hair cut!\n");
    sleep(10);
    printf("Customer: Finished getting hair cut!\n");
}

// Representation of barber thread
void barber(void *queue) {
    int i;
    int customerNumber;

    // Keep checking customers, cutting hair, and sleeping
    while (1) {
        i = 0;
        customerNumber = theQueue.customerCount;

        while (theQueue.customerCount == 0) {
            printf("Barber: Sleeping\n");
            sleep(5);
        }

        for (i = 0; i < customerNumber; i++) {
            // Call cut hair
            cut_hair();
        }
    }
}

// Representation of customer thread
 void customer(void *queue) {

    // Check for space in queue
    if (theQueue.customerCount >= theQueue.chairs) {
        printf("Queue is full: Customer is leaving.\n");
        return;
    }

    // Set locks and push
    pthread_mutex_lock(&customerLock);
    queue_push();
    pthread_mutex_unlock(&customerLock);

    printf("Customer: In chair waiting\n");

    // Set lock and call get_hair_cut()
    pthread_mutex_lock(&barberLock);
    get_hair_cut();

    // Pop from queue and unlock
    queue_pop();
    pthread_mutex_unlock(&barberLock);
}

int main(int argc, char **argv) {

    // Instantiate threads and functions
    struct chair mainChair;
    pthread_t barberThread;
    pthread_t customer1, customer2, customer3, customer4;
    void *barberFunction = barber;
    void *customerFunction = customer;

    // Set up signals
    struct sigaction sig;
    sig.sa_flags = 0;
    sig.sa_handler = signalCatch;
    sigaction(SIGINT, &sig, NULL);

    mainChair.next = NULL;
    theQueue.current = &mainChair;

    // Set mutex locks
    pthread_mutex_init(&barberLock, NULL);
    pthread_mutex_init(&customerLock, NULL);

    // Set chairs and customers
    theQueue.customerCount = 0;
    theQueue.chairs = 3;

    // Create threads
    pthread_create(&barberThread, NULL, barberFunction, NULL);
    sleep(5);

    pthread_create(&customer1, NULL, customerFunction, NULL);
    pthread_create(&customer2, NULL, customerFunction, NULL);
    pthread_create(&customer3, NULL, customerFunction, NULL);
    pthread_create(&customer4, NULL, customerFunction, NULL);

    // Run forever
    while (1) {

    }
}
