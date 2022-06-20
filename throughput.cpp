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

int main(int argc, char* argv[]) {
	int threadcount = 6;
	if (argc>1) threadcount = atoi(argv[1]);

    std::atomic<word_t> ato{4*4},ato2{4*3},ato3{3}, ato4{4*3}, ato5{4*3}, ato6{4*3}, ato7{4*3}, ato8{4*3}, fail_a{2*4}, succ9_a{3*4};
	CASN_entry entry1{&ato, 4*4, 6*4}, entry2{&ato2,4*3,4*6}, entry3{&ato3,4*3,4*6}, entry4{&ato4,4*3,4*6}, entry5{&ato5,4*3,4*6}, entry6{&ato6,4*3,4*6}, entry7{&ato7,4*3,4*4}, entry8{&ato8,4*3,4*3};
	CASN_entry entry2_0{&ato, 6*4, 4*4}, entry2_2{&ato2, 4*6,4*3}, entry2_3{&ato3, 4*6,3*4}, entry2_4{&ato4, 4*6,4*3}, entry2_5{&ato5, 4*6,4*3}, entry2_6{&ato6, 4*6,4*3}, entry2_7{&ato7, 4*4,4*3}, entry2_8{&ato8, 4*3,4*3};
    
    CASN_entry entry_thread1{&ato3,3,4*6}, entry_thread2{&ato3,3,4*6};
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;
    std::vector<int> casn_throughput;
    std::vector<int> cas_throughput;

	
    omp_set_dynamic(0);
    omp_set_num_threads(threadcount);
    int loops = 10;
    std::vector<int> ran_shared(loops*threadcount,0);
    std::vector<int> ran_shared2(loops*threadcount,0);
    std::vector<int> timings_global(threadcount*8,0);
  
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
    	
		std::atomic<word_t> succ0_a{3*4}, succ1_a{3*4}, succ2_a{3*4}, succ3_a{3*4}, succ4_a{3*4}, succ5_a{3*4}, succ6_a{3*4}, succ7_a{3*4}, succ8_a{3*4};
		CASN_entry succ0{&succ0_a,3*4,4*4}, succ1{&succ1_a,3*4,4*4}, succ2{&succ2_a,3*4,4*4}, succ3{&succ3_a,3*4,4*4}, succ4{&succ4_a,3*4,4*4}, succ5{&succ5_a,3*4,4*4}, succ6{&succ6_a,3*4,4*4}, succ7{&succ7_a,3*4,4*4}, succ8{&succ8_a,3*4,4*4}, fail{&fail_a,4*4,3*4};
		std::vector<CASN_entry> fail_first, fail_last;
		#pragma omp barrier
		
		CASN_entry shared_0{&succ9_a,3*4,3*4}, succ0_b{&succ0_a,4*4,3*4}, succ1_b{&succ1_a,4*4,3*4}, succ2_b{&succ2_a,4*4,3*4}, succ3_b{&succ3_a,4*4,3*4}, succ4_b{&succ4_a,4*4,3*4}, succ5_b{&succ5_a,4*4,3*4}, succ6_b{&succ6_a,4*4,3*4}, succ7_b{&succ7_a,4*4,3*4}, succ8_b{&succ8_a,4*4,3*4}, succ9{&succ9_a,3*4,3*4}, succ9_b{&succ9_a, 3*4,3*4};

		std::vector<CASN_entry> succ_entries, succ_entries2;
		succ_entries.insert(succ_entries.end(), {succ9, succ0, succ1, succ2, succ3, succ4, succ5, succ6, succ7, succ8});
		succ_entries2.insert(succ_entries2.end(), {succ9_b, succ0_b, succ1_b, succ2_b, succ3_b, succ4_b, succ5_b, succ6_b, succ7_b, succ8_b});
		
		
		//throughput testing
		succ0_a = 3*4;
		succ1_a = 3*4;
		succ2_a = 3*4;
		succ3_a = 3*4;
		succ4_a = 3*4;
		succ5_a = 3*4;
		succ6_a = 3*4;
		succ7_a = 3*4;
		succ8_a = 3*4;
		succ9_a = 3*4;
		
		
		auto start = std::chrono::high_resolution_clock::now();
		int output=0;
		while(std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - start).count() < 3000) {
			bool ran = CASN(succ_entries);
			bool ran2 = CASN(succ_entries2);
			if(!ran || !ran2) {std::cout << "failed throughput testing at iteration " << output << " thread " << id << " values in the descriptor" << std::endl;
			std::cout << succ0_a << succ1_a << succ2_a << succ3_a << succ4_a << succ5_a << succ6_a << succ7_a << succ8_a << succ9_a << std::endl;
			output=0;
			break;
			}
			output++;
		}
		timings_global[7*threadcount+id] = output;
		
		int a=3;
		int b=4;
		output=0;
		std::atomic<word_t>temp{3};
		start = std::chrono::high_resolution_clock::now();
		while(std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - start).count() < 3000) {
			CAS1(temp,a,b);
			CAS1(temp,b,a);
			output++;
		}
		timings_global[8*threadcount+id] = output/10;
		
		
    }
    /*
    std::cout << "minimum runtimes:" << std::endl;
	for(int i=0;i<threadcount;i++){
	std::cout << "thread " << i << " " << timings_global[i] << std::endl;
	}
	std::cout << "median runtimes:" << std::endl;
	for(int i=0;i<threadcount;i++){
	std::cout << "thread " << i << " " << timings_global[threadcount + i] << std::endl;
	}
	std::cout << "average runtimes:" << std::endl;
	for(int i=0;i<threadcount;i++){
	std::cout << "thread " << i << " " << timings_global[2*threadcount + i] << std::endl;
	}
	std::cout << "maximum runtimes:" << std::endl;
	for(int i=0;i<threadcount;i++){
	std::cout << "thread " << i << " " << timings_global[3*threadcount + i] << std::endl;
	}*/
	std::ofstream outputfile;
	outputfile.open("throughput" + std::to_string(threadcount) +".txt");
	outputfile << "throughput,throughput_base" << std::endl;
	for(int i=0;i<threadcount;i++){
	outputfile << timings_global[7*threadcount +i] << "," << timings_global[8*threadcount +i] << std::endl;
	}
	outputfile.close();
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}
