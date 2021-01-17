EXE = ../bin/hw5

GCC = g++ 
GCCFLAG = -std=c++11 -fopenmp -O3 -Wno-unused-result

SRC = main.cpp Router.cpp Path.cpp Net.cpp

all: $(SRC)
	$(GCC) $(GCCFLAG) $^ -o $(EXE)
test:
	$(EXE) ../testcase/ibm01.modified.txt ../output/ibm01.result
	$(EXE) ../testcase/ibm04.modified.txt ../output/ibm04.result
clean:
	rm -f $(EXE) *.o *.gch