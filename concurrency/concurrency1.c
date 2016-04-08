#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "mt19937ar.c"

int eax, ebx, ecx, edx;
int producerIndex, consumerIndex;

struct bufferItem {
    int number;
    int sleepTime;
};

struct bufferArray {
    struct bufferItem buffer[32];
};

void produce() {

}

void consume() {

}

void setRegisters() {
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

    number %= (max - min);
    if (number < min) {
        number = min;
    }
    printf("MT number: %d\n", number);
}

int main(int argc, char *argv) {
   printf("In - main()\n");
   while(1) {
       randomNumberGenerator(1, 100);
   }
}
