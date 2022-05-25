#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <atomic>
#include "casn.cpp"

int main(int argc,char *argv[]){
	int N=2;
	Descriptor<double> desc;
	desc.resize(N);
	std::atomic<double> value=1.0;
	double exp = 2.0;
	double des = 3.0;
	bool succ = value.compare_exchange_weak(exp ,des ,std::memory_order_release, std::memory_order_relaxed);
	std::cout << "succ " << succ << std::endl;
	std::cout << "value " << value << std::endl;
	std::cout << "exp " << exp << std::endl;
	std::cout << "des " << des << std::endl;
	std::cout << "testing" << std::endl;
	exp = 2.0;
	double val = 1.0;
	exp = CAS1<double>(&val, exp, des);
	std::cout << "value " << val << std::endl;
	std::cout << "exp " << exp << std::endl;
	std::cout << "des " << des << std::endl;
	return 1;
}
