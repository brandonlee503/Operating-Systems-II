#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "mt19937ar.c"

//Global variables
struct linkedList list;
int inserters, searchers, deleters, number;

struct node {
    int value;
    struct node *next;
};

struct linkedList {
    int nodes;
    struct node *head;
    struct node *current;
};

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

// Cleanup and exit
void signalCatch(int signal) {
    printf("Catch signal: %d\n", signal);
    kill(0, signal);
    exit(0);
}

void inserter(/* arguments */) {
    /* code */
}

void searcher(/* arguments */) {
    /* code */
}

void deleter(/* arguments */) {
    /* code */
}

int main(int argc, char const *argv[]) {
    /* code */
    return 0;
}
