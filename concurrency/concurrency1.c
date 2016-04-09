#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
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

bool bufferHasSpace() {
    int i;
    for (i = 0; i < 32; i++) {
        if (buffer.buffer[i].number == 0) {
            return true;
        }
    }
    return false;
}

void *produce() {
    printf("In - produce()\n");
    struct bufferData bufferValue;
    int dataNumber;
    int dataSleepTime;

    while(1) {
        // Lock the routine from other threads
        pthread_mutex_lock(&buffer.lock);
        // while (producerIndex == 31) {
        //     // Blocks calling thread until producer condition is signalled.
        //     pthread_cond_wait(&producerCondition, &buffer.lock);
        // }
        // FIXME: experimenting
        while (!bufferHasSpace()) {
            // Block calling thread until consumer condition is signalled
            pthread_cond_wait(&producerCondition, &buffer.lock);
        }

        // Create random data number and sleep time
        // dataNumber = randomNumberGenerator(1, 100);
        // dataSleepTime = randomNumberGenerator(2, 9);

        // Insert data into buffer
        // bufferValue.number = dataNumber;
        // bufferValue.sleepTime = dataSleepTime;
        // buffer.buffer[producerIndex] = bufferValue;
        // FIXME: experimenting
        if (buffer.buffer[producerIndex].number == 0) {
            // Create random data number and sleep time
            dataNumber = randomNumberGenerator(1, 100);
            dataSleepTime = randomNumberGenerator(2, 9);
            // Insert data into buffer
            bufferValue.number = dataNumber;
            bufferValue.sleepTime = dataSleepTime;
            buffer.buffer[producerIndex] = bufferValue;
        }

        // Increment and check index
        producerIndex++;
        if (producerIndex >= 32) {
            producerIndex = 0;
        }

        // sleep(randomNumberGenerator(3, 7));

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
        // Block calling thread until consumer condition is signalled
        pthread_cond_wait(&consumerCondition, &buffer.lock);
    }

    // Get data from buffer and increment index
    bufferValue = buffer.buffer[consumerIndex];
    consumerIndex++;
    if (consumerIndex >= 32) {
        consumerIndex = 0;
    }

    // Get values from data
    dataNumber = bufferValue.number;
    dataSleepTime = bufferValue.sleepTime;

    // Sleep
    sleep(dataSleepTime);
    printf("Consumption Value: %d\n", dataNumber);

    // Print buffer state
    printf("Buffer State: ");
    int i;
    int bufferSize = sizeof(buffer.buffer)/sizeof(buffer.buffer[0]);
    for (i = 0; i < 32; i++) {
        printf("%d ", buffer.buffer[i]);
    }
    printf("\n");

    // Wake up producer thread and unlock mutex
    pthread_cond_signal(&producerCondition);
    pthread_mutex_unlock(&buffer.lock);
}

// Cleanup and exit
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
   // pthread_create(&producer1, NULL, produce, NULL);

   while(1) {
       pthread_create(&consumer0, NULL, consume, NULL);
    //    pthread_create(&consumer1, NULL, consume, NULL);
       pthread_join(consumer0, NULL);
   }
}
