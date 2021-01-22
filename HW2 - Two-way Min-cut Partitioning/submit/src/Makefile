all:
	g++ -std=c++11 main.cpp -o ../bin/hw2
clean:
	rm ../bin/hw2
testall:
	/usr/bin/time -p ../bin/hw2 ../testcases/p2-1.nets ../testcases/p2-1.cells ../output/p2-1.out -m
	../verifier/verify ../testcases/p2-1.nets ../testcases/p2-1.cells ../output/p2-1.out
	/usr/bin/time -p ../bin/hw2 ../testcases/p2-2.nets ../testcases/p2-2.cells ../output/p2-2.out -m
	../verifier/verify ../testcases/p2-2.nets ../testcases/p2-2.cells ../output/p2-2.out
	/usr/bin/time -p ../bin/hw2 ../testcases/p2-3.nets ../testcases/p2-3.cells ../output/p2-3.out -m
	../verifier/verify ../testcases/p2-3.nets ../testcases/p2-3.cells ../output/p2-3.out
	/usr/bin/time -p ../bin/hw2 ../testcases/p2-4.nets ../testcases/p2-4.cells ../output/p2-4.out -m
	../verifier/verify ../testcases/p2-4.nets ../testcases/p2-4.cells ../output/p2-4.out
	/usr/bin/time -p ../bin/hw2 ../testcases/p2-5.nets ../testcases/p2-5.cells ../output/p2-5.out -m
	../verifier/verify ../testcases/p2-5.nets ../testcases/p2-5.cells ../output/p2-5.out
hidden:
	/usr/bin/time -p ../bin/hw2 ../testcases/h2-1.nets ../testcases/h2-1.cells ../output/h2-1.out -m
	../verifier/verify ../testcases/h2-1.nets ../testcases/h2-1.cells ../output/h2-1.out
	/usr/bin/time -p ../bin/hw2 ../testcases/h2-2.nets ../testcases/h2-2.cells ../output/h2-2.out -m
	../verifier/verify ../testcases/h2-2.nets ../testcases/h2-2.cells ../output/h2-2.out
	/usr/bin/time -p ../bin/hw2 ../testcases/h2-3.nets ../testcases/h2-3.cells ../output/h2-3.out -m
	../verifier/verify ../testcases/h2-3.nets ../testcases/h2-3.cells ../output/h2-3.out
