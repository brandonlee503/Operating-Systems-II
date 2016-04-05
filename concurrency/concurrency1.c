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

int randomNumberGenerator() {
    
}

int main(int argc, char **argv) {
    
}
