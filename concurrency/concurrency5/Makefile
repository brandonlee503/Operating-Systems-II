# NOTE: If running into an error "make: Nothing to be done for `all'", make sure the testing directory only has CigaretteSmoker.java and Makefile!
all: concurrency5

concurrency5:
	javac CigaretteSmokers.java
	java CigaretteSmokers

clean:
	rm -f *.pdf *.ps *.dvi *.out *.log *.aux *.bbl *.blg *.pyg *.o *.out *.log *.aux *~ *.class 

.PHONY: all show clean ps pdf showps
