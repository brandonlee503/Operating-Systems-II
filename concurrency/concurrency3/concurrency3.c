#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "mt19937ar.c"

// Global variables
struct linkedList list;
int eax, ebx, ecx, edx;
int searchers, inserters, deleters, number;

pthread_mutex_t searchLock;
pthread_mutex_t insertLock;
pthread_mutex_t deleteLock;

struct linkedList {
    int value;
    struct node *next;
} *head;

// The following 3 functions are essentially the same as my previous implementation in Assignment 1.
// Boilerplate for setting up ASM registers
void setRegisters() {
    // Set eax else core dump
    eax = 0x01;
    // http://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
    __asm__ __volatile__("cpuid;" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(eax));
}

// https://hackage.haskell.org/package/crypto-random-0.0.9/src/cbits/rdrand.c
int rdrand(int *number) {
    unsigned char err;
    __asm__ __volatile__("rdrand %0; setc %1" : "=r" (*number), "=qm" (err));
    return (int) err;
}

int randomNumberGenerator(int min, int max) {
    int number = 0;
    setRegisters();

    // Check if system supports rdrand by checking bit 30 of ECX register.
    // http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-manual-325462.html
    // http://stackoverflow.com/questions/523724/c-c-check-if-one-bit-is-set-in-i-e-int-variable
    if (ecx & 1<<30) {
        rdrand(&number);
    } else {
        number = (int)genrand_int32();
    }
    number = abs(number);
    number %= (max - min);
    if (number < min) {
        number = min;
    }

    return number;
}

void *searcher() {
    struct linkedList *searchLinkedList;
    while (1) {
        if (!pthread_mutex_trylock(&searchLock)) {
            searchLinkedList = head;
            if (searchLinkedList == NULL) {
                printf("The list is empty!\n");
                continue;
            } else {
                while (searchLinkedList != NULL) {
                    printf("%d\n", searchLinkedList->value);
                    searchLinkedList = searchLinkedList->next;
                }
                printf("\n");
            }
            pthread_mutex_unlock(&searchLock);
        }
        sleep(1);
    }
}

void *inserter() {
    int randomNumber;
    struct linkedList insertLinkedList, **tail;
    while (1) {
        if (linkedListSize < 20) {
            if (!pthread_mutex_trylock(&insertLock)) {
                randomNumber = randomNumberGenerator(1, 10);
                insertLinkedList = (struct linkedList *)malloc(sizeof(struct linkedList));

                printf("Inserting value: %d\n", randomNumber);
                insertLinkedList->value = randomNumber;
                insertLinkedList->next = NULL;
                tail = &head;

                if (head == NULL) {
                    head == insertLinkedList;
                } else {
                    // TODO: Make this not suck
                    while (*tail != NULL) {
                        tail = &((*tail)->next);
                    }

                    *tail = insertLinkedList;
                }

                pthread_mutex_unlock(&insertLock);
                // TODO: Update times
                sleep(5);
            }
        }
    }
}

void *deleter(/* arguments */) {
    /* code */
}

int main(int argc, char const *argv[]) {

    // Instantiate threads
    pthread_t searchThread[3], insertThread[3], deleteThread[3];

    // Instantiate linked list
    struct linkedList *theLinkedList;
    theLinkedList = (struct linkedList *)malloc(sizeof(struct linkedList));
    theLinkedList->value = randomNumberGenerator(1, 10);
    head = theLinkedList;
    head->next = NULL;

    // Create search, insert, and delete pthreads
    int i;
    for (i = 0; i < 3; i++) {
        pthread_create(&searchThread[i], NULL, searcher, NULL);
        pthread_create(&insertThread[i], NULL, inserter, NULL);
        pthread_create(&deleteThread[i], NULL, deleter, NULL);
    }

    // Start up!
    for (i = 0; i < 3; i++) {
        pthread_join(searchThread[i], NULL);
        pthread_join(insertThread[i], NULL);
        pthread_join(deleteThread[i], NULL);
    }

    return 0;
}
