#pragma once
#include <vector>
/*
template<typename T>
class Descriptor{
	public:
	int N;
	std::vector<T*> control_address;
	std::vector<T> expected_values;
	std::vector<T*> data_address;
	std::vector<T> old_values; 
	std::vector<T> new_values;
	void resize(int size);
};
*/

//template<typename T>
class Descriptor{
	public:
	int N;
	std::vector<double*> control_address;
	std::vector<double> expected_values;
	std::vector<double*> data_address;
	std::vector<double> old_values; 
	std::vector<double> new_values;
	void resize(int size);
};

