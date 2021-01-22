EXE = ../bin/hw3

GCC = g++ 
GCCFLAG = -std=c++11 -fopenmp -O3 -Wno-unused-result

SRC = main.cpp Floorplan.cpp Parser.cpp BStree.cpp Contour.cpp
TESTCASE = ../testcase/
OUTPUT = ../output/

all: $(SRC)
	$(GCC) $(GCCFLAG) $^ -o $(EXE)

clean:
	rm -f $(EXE) *.o *.gch
