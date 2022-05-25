#include <vector>
template<typename T>
class Descriptor{
	public:
	int N;
	std::vector<T*> control_address;
	std::vector<T> expected_values;
	std::vector<T*> data_address;
	std::vector<T> old_values; 
	std::vector<T> new_values;
	void resize(int size){
		N=size;
		control_address.resize(N);
		expected_values.resize(N);
		data_address.resize(N);
		old_values.resize(N);
		new_values.resize(N);
	};
};

template<typename T>
T CAS1(T *a, T o, T n){
T old = *a;
if(old==o) *a=n;
return old;
};
