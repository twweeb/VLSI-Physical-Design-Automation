all:
	g++ -std=c++11 main.cpp BStree.cpp -o ../bin/hw3
test:
	../bin/hw3 ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100_1.floorplan 0.1 -m -d ../output/n100_1.fig
	../bin/hw3 ../testcase/n100.hardblocks ../testcase/n100.nets ../testcase/n100.pl ../output/n100_15.floorplan 0.15 -m -d ../output/n100_15.fig
	../bin/hw3 ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl ../output/n200_1.floorplan 0.1 -m -d ../output/n200_1.fig
	../bin/hw3 ../testcase/n200.hardblocks ../testcase/n200.nets ../testcase/n200.pl ../output/n200_15.floorplan 0.15 -m -d ../output/n200_15.fig
	../bin/hw3 ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl ../output/n300_1.floorplan 0.1 -m -d ../output/n300_1.fig
	../bin/hw3 ../testcase/n300.hardblocks ../testcase/n300.nets ../testcase/n300.pl ../output/n300_15.floorplan 0.15 -m -d ../output/n300_15.fig
clean:
	rm ../bin/hw3
