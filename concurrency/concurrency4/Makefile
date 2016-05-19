all: concurrency4part1 concurrency4part2

concurrency4part1: concurrency4part1.o
	gcc concurrency4part1.o -o concurrency4part1 -pthread

concurrency4part1.o:
	gcc -c concurrency4part1.c -g

concurrency4part2: concurrency4part2.o
	gcc concurrency4part2.o -o concurrency4part2 -pthread

concurrency4part2.o:
	gcc -c concurrency4part2.c -g

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg concurrency4part1 concurrency4part2 *.o *.out *.log *.aux *~

.PHONY: all show clean ps pdf showps
