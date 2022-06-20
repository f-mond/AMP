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

    std::atomic<word_t> ato{4*4},ato2{4*3},ato3{3*4}, ato4{4*3}, ato5{4*3}, ato6{4*3}, ato7{4*3}, ato8{4*3};
	CASN_entry entry1{&ato, 4*4, 6*4}, entry2{&ato2,4*3,4*6}, entry3{&ato3,4*3,4*6}, entry4{&ato4,4*3,4*6}, entry5{&ato5,4*3,4*6}, entry6{&ato6,4*3,4*6}, entry7{&ato7,4*3,4*4}, entry8{&ato8,4*3,4*3};
	CASN_entry entry2_0{&ato, 6*4, 4*4}, entry2_2{&ato2, 4*6,4*3}, entry2_3{&ato3, 4*6,3*4}, entry2_4{&ato4, 4*6,4*3}, entry2_5{&ato5, 4*6,4*3}, entry2_6{&ato6, 4*6,4*3}, entry2_7{&ato7, 4*4,4*3}, entry2_8{&ato8, 4*3,4*3};
    
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;

	
    omp_set_dynamic(0);
    omp_set_num_threads(threadcount);
    int loops = 10;
    std::vector<int> ran_shared(threadcount,0);
    std::vector<int> ran_shared2(threadcount,0);
    #pragma omp parallel
    {
    	//starting correctness test
        int id = omp_get_thread_num();
        std::atomic<word_t> ato9{4*4}, ato10{4*4};
	    CASN_entry entry9{&ato9,4*4,4*4}, entry10{&ato10,4*4,4*4};
	    CASN_entry entry2_9{&ato9, 4*4,4*4}, entry2_10{&ato10,4*4,4*4};
	    std::vector<CASN_entry> ent;
	    std::vector<CASN_entry> ent2;
	    std::vector<int> exp_values(10,0);

		ent.push_back(entry1);
		ent2.push_back(entry2_0);

		ent.push_back(entry2);
		ent2.push_back(entry2_2);
		/*
		ent.push_back(entry3);
		ent2.push_back(entry2_3);

		ent.push_back(entry4);
		ent2.push_back(entry2_4);

		ent.push_back(entry5);
		ent2.push_back(entry2_5);

		ent.push_back(entry6);
		ent2.push_back(entry2_6);

	    ent.push_back(entry7);
	    ent2.push_back(entry2_7);

	    ent.push_back(entry8);
	    ent2.push_back(entry2_8);

	    ent.push_back(entry9);
	    ent2.push_back(entry2_9);

	    ent.push_back(entry10);
	    ent2.push_back(entry2_10);
*/
        
		bool ran, ran2;

		for(int j=0;j<loops;j++){
		
			#pragma omp barrier
			for(int i=0;i<ent.size();i++){
			exp_values[i]=*ent[i].addr;
			}
			ran=0;
			#pragma omp barrier
			ran = CASN(ent);
			ran_shared[id] = ran;
			#pragma omp barrier
			int i=0;
			

			bool non_sharing_thread_ran;
			int any_thread_ran=0;
			#pragma omp barrier
			for(int k=0;k<threadcount;k++){
			if(ran_shared[k]==1){std::cout << id;}}
			std::for_each(ran_shared.begin(), ran_shared.end(), [&] (int n) {any_thread_ran += n;});
			#pragma omp barrier
			for(int i=0;i<ent.size();i++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if((ran && *ent[i].addr!=ent[i].new1) || (!ran && *ent[i].addr != exp_values[i] && !any_thread_ran) && abs(*ent[i].addr)<100) 
			{
				if((ran && *ent[i].addr!=ent[i].new1) || (!ran && *ent[i].addr != exp_values[i] && !any_thread_ran) && abs(*ent[i].addr)<100) std::cout << "check failed thread " << id << " on entry " << i << " should be " << (ran? ent[i].new1 : exp_values[i]) << " but is " << (*ent[i].addr) << " " << any_thread_ran << " " << ran << std::endl;
			}
			}
			#pragma omp barrier
			if(any_thread_ran==0) {
			if(id==0){
			std::cout << "no thread ran op 1 " << id << " " << j << std::endl;
			for(int i=0;i<ent.size();i++) {
				std::cout << *ent[i].addr << " ";
			}
			std::cout << std::endl << std::endl;
			}
			}
			if(any_thread_ran>1 && id==0) std::cout << "multiple threads ran " << any_thread_ran << " " << id << " " << j << std::endl;
			
			#pragma omp barrier
			exp_values[0]=*entry1.addr;
			exp_values[1]=*entry2.addr;
			exp_values[2]=*entry3.addr;
			exp_values[3]=*entry4.addr;
			exp_values[4]=*entry5.addr;
			exp_values[5]=*entry6.addr;
			exp_values[6]=*entry7.addr;
			exp_values[7]=*entry8.addr;
			exp_values[8]=*entry9.addr;
			exp_values[9]=*entry10.addr;
			#pragma omp barrier
			
			ran2 = CASN(ent2);
			ran_shared2[id]=ran2;
			#pragma omp barrier
			i=0;
			
			any_thread_ran = 0;
			std::for_each(ran_shared.begin(), ran_shared.end(), [&] (int n) {any_thread_ran += n;});
			for(int i=0;i<ent.size();i++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if(((ran2 && *ent2[i].addr!=ent2[i].new1) || (!ran2 && *ent2[i].addr != exp_values[i] && !any_thread_ran)) && abs(*ent2[i].addr)<100) std::cout << "check failed operation 2 thread " << id << " on entry " << i << " should be " << (ran2? ent2[i].new1 : exp_values[i]) << " but is " << (*ent2[i].addr) << " " << id << std::endl;
			}
			#pragma omp barrier
			if(any_thread_ran==0) std::cout << "no thread ran " << id << " " << j << std::endl;
			if(any_thread_ran>1 && id==0) std::cout << "multiple threads ran " << any_thread_ran << " " << id << " " << j << std::endl;
			if(ran) std::cout << " 1" << id;
			if(ran2) std::cout << " 2" << id;
			#pragma omp barrier
			if(id==0)std::cout << " loop " << j << " done" << std::endl << std::endl;
		}
		#pragma omp barrier
		if(id==0) std::cout << std::endl;
		}
	return 0;
}
