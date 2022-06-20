#include <vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "casn2.cpp" 
#include <chrono>
#include <fstream>
#include <thread>

int main(int argc, char* argv[]) {
	int threadcount = 6;
	if (argc>1) threadcount = atoi(argv[1]);

    std::atomic<word_t> ato{4*3},ato2{4*3},ato3{3*4}, ato4{4*3}, ato5{4*3}, ato6{4*3}, ato7{4*3}, ato8{4*3};

    
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;

	
    omp_set_dynamic(0);
    omp_set_num_threads(threadcount);
    int loops = 10;
    std::vector<bool> ran_shared(threadcount,0);
    #pragma omp parallel
    {
    	//starting correctness test
        int id = omp_get_thread_num();

	    std::vector<CASN_entry> ent;
	    std::atomic<word_t> local{4*4};
        CASN_entry entry1{&ato,4*3,4*id}, entry2{&ato2, 4*3, 4*4};
	    CASN_entry entry_local{&local, 4*4, 4*id};

		ent.push_back(entry1);
		ent.push_back(entry2);
		ent.push_back(entry_local);
        
		bool ran;

		ran=0;
		#pragma omp barrier
		ran = CASN(ent);
		ran_shared[id] = ran;
		#pragma omp barrier
		int thread_that_ran;
		for(int i=0;i<threadcount;i++){if(ran_shared[i]==true && i==id){ std::cout << "thread " << i << " ran" << std::endl;
		std::cout << ato << std::endl;}}
		if((ran && *ent[0].addr != ent[0].new1 && *ent[1].addr != ent[1].new1) || (!ran && *ent[0].addr != ent[0].new1 && *ent[1].addr != 4*4)) std::cout << "failure" << std::endl;
		int number_of_threads_ran=0;
		std::for_each(ran_shared.begin(), ran_shared.end(), [&] (int n) {number_of_threads_ran += n;});
		if(number_of_threads_ran>1) std::cout << "failure, more than one ran " << number_of_threads_ran << std::endl;
		if(ran)std::cout << "thread " << id << " result " << CASNRead(&ato) << " " << local << std::endl;
		
		if(ran) ran=CASN(ent);
		if(ran) std::cout << "supergau";
		
		}
	return 0;
}
