all: concurrency1

concurrency1: concurrency1.o
	gcc concurrency1.o -o assignment1 -pthread

concurrency1.o:
	gcc -c concurrency1.c -g

clean:
	rm -rf assignment1 *.o *.out *.log *.aux *~
