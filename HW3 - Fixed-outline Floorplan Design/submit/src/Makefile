EXE = ../bin/hw3

GCC = g++ 
GCCFLAG = -std=c++11 -fopenmp -O3 -Wno-unused-result

SRC = main.cpp Floorplan.cpp Parser.cpp BStree.cpp Contour.cpp

all: $(SRC)
	$(GCC) $(GCCFLAG) $^ -o $(EXE)
test_no_seed:
	$(EXE) ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100_1.floorplan 0.1 -d ../output/n100_1.fig
	$(EXE) ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100_15.floorplan 0.15 -d ../output/n100_15.fig
	$(EXE) ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl ../output/n200_1.floorplan 0.1 -d ../output/n200_1.fig
	$(EXE) ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl ../output/n200_15.floorplan 0.15 -d ../output/n200_15.fig
	$(EXE) ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl ../output/n300_1.floorplan 0.1 -d ../output/n300_1.fig
	$(EXE) ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl ../output/n300_15.floorplan 0.15 -d ../output/n300_15.fig
test_seed:
	$(EXE) ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100_1.floorplan 0.1 -s -d ../output/n100_1.fig
	$(EXE) ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100_15.floorplan 0.15 -s -d ../output/n100_15.fig
	$(EXE) ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl ../output/n200_1.floorplan 0.1 -s -d ../output/n200_1.fig
	$(EXE) ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl ../output/n200_15.floorplan 0.15 -s -d ../output/n200_15.fig
	$(EXE) ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl ../output/n300_1.floorplan 0.1 -s -d ../output/n300_1.fig
	$(EXE) ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl ../output/n300_15.floorplan 0.15 -s -d ../output/n300_15.fig
clean:
	rm -f $(EXE) *.o *.gch