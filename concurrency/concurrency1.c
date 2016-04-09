#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "mt19937ar.c"

int eax, ebx, ecx, edx;
int producerIndex, consumerIndex;
pthread_cond_t producerCondition, consumerCondition;

struct bufferData {
    int number;
    int sleepTime;
};

struct bufferArray {
    struct bufferData buffer[32];
    pthread_mutex_t lock; // Mutex lock for thread synchronization
};

struct bufferArray buffer;

void *produce() {
    printf("In - produce()\n");
    struct bufferData bufferValue;
    int dataNumber;
    int dataSleepTime;

    while(1) {
        // Lock the routine from other threads
        pthread_mutex_lock(&buffer.lock);
        while (producerIndex == 31) {
            // Blocks the calling thread until the specified condition is signalled.
            pthread_cond_wait(&producerCondition, &buffer.lock);
        }

        // Create random data number and sleep time
        dataNumber = randomNumberGenerator(1, 100);
        // printf("dataNumber: %d\n", dataNumber);
        dataSleepTime = randomNumberGenerator(2, 9);
        // printf("dataSleepTime: %d\n", dataSleepTime);

        // Insert data into buffer
        bufferValue.number = dataNumber;
        bufferValue.sleepTime = dataSleepTime;
        buffer.buffer[producerIndex] = bufferValue;

        // Increment and check index
        producerIndex++;
        if (producerIndex >= 32) {
            producerIndex = 0;
        }

        // Wake up consumer thread and unlock mutex
        pthread_cond_signal(&consumerCondition);
        pthread_mutex_unlock(&buffer.lock);
    }
}

void *consume() {
    printf("In - consume()\n");
    struct bufferData bufferValue;
    int dataNumber;
    int dataSleepTime;

    // Lock routine from other threads
    pthread_mutex_lock(&buffer.lock);
    while (producerIndex == 0) {
        pthread_cond_wait(&consumerCondition, &buffer.lock);
    }

    bufferValue = buffer.buffer[consumerIndex];
    consumerIndex++;
    if (consumerIndex >= 32) {
        consumerIndex = 0;
    }

    dataNumber = bufferValue.number;
    dataSleepTime = bufferValue.sleepTime;
    // printf("dataSleepTime: %d\n", dataSleepTime);

    sleep(dataSleepTime);
    printf("Value %d\n", dataNumber);
    // printf("Buffer: %d\n", buffer.buffer[0].value);
    pthread_cond_signal(&producerCondition);
    pthread_mutex_unlock(&buffer.lock);
}

void signalCatch(int signal) {
    printf("Catch signal: %d\n", signal);
    pthread_mutex_destroy(&buffer.lock);

    pthread_cond_destroy(&producerCondition);
    pthread_cond_destroy(&consumerCondition);

    kill(0, signal);
    exit(0);
}

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
    // printf("In - randomNumberGenerator()\n");
    int number = 0;
    setRegisters();

    // Check if system supports rdrand by checking bit 30 of ECX register.
    // http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-manual-325462.html
    // http://stackoverflow.com/questions/523724/c-c-check-if-one-bit-is-set-in-i-e-int-variable
    if (ecx & 1<<30) {
        printf("rdrand ");
        rdrand(&number);
    } else {
        printf("MT ");
        number = (int)genrand_int32();
    }
    number = abs(number);
    number %= (max - min);
    if (number < min) {
        number = min;
    }
    printf("number: %d\n", number);
    return number;
}

int main(int argc, char *argv) {
   printf("In - main()\n");

   pthread_t consumer0, consumer1, producer0, producer1;

   struct sigaction act;
   producerIndex = 0;
   consumerIndex = 0;

   // Set up signal catching
   sigemptyset(&act.sa_mask);
   act.sa_flags = 0;
   act.sa_handler = signalCatch;
   sigaction(SIGINT, &act, NULL);

   // https://computing.llnl.gov/tutorials/pthreads/
   pthread_cond_init(&producerCondition, NULL);
   pthread_cond_init(&consumerCondition, NULL);
   pthread_mutex_init(&buffer.lock, NULL);
   pthread_create(&producer0, NULL, produce, NULL);
   pthread_create(&producer1, NULL, produce, NULL);

   while(1) {
    //    randomNumberGenerator(2, 9);
       pthread_create(&consumer0, NULL, consume, NULL);
       pthread_create(&consumer1, NULL, consume, NULL);
       pthread_join(consumer0, NULL);
   }
}
