EXE = ../bin/hw2
GCC = g++
GCCFLAG = -std=c++11 -fopenmp -O3 -Wno-unused-result
SRC = main.cpp

all: $(SRC)
	$(GCC) $(GCCFLAG) $^ -o $(EXE)

clean:
	rm -f $(EXE) *.o *.gch
