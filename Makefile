all:
	g++ casn_ferdi.cpp -std=c++14 -fopenmp -o test
	g++ throughput.cpp -std=c++14 -fopenmp -o throughput
	g++ correctness.cpp -std=c++14 -fopenmp -o correctness
