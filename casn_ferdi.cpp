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
    std::vector<int> timings_global(threadcount*6,0);
  
    #pragma omp parallel
    {
    
    	//starting correctness test
        int id = omp_get_thread_num();
        std::atomic<word_t> ato9{4*4}, ato10{4*4};
	    CASN_entry entry9{&ato9,4*4,4*5}, entry10{&ato10,4*4,4*5};
	    CASN_entry entry2_9{&ato9, 4*5,4*4}, entry2_10{&ato10,4*5,4*4};
	    std::vector<CASN_entry> ent;
	    std::vector<CASN_entry> ent2;
	    std::vector<int> exp_values(10,0);
	    std::vector<bool> incl(10,0);
	    std::vector<bool> incl2(10,0);
        if(id%2==0){
			ent.push_back(entry1);
			ent2.push_back(entry2_0);
			incl[0] = 1;
			incl2[0] = 1;

			ent.push_back(entry2);
			ent2.push_back(entry2_2);
			incl[1] = 1;
			incl2[1] = 1;

			ent.push_back(entry3);
			//ent.push_back(entry_thread2);
			ent2.push_back(entry2_3);
			incl[2] = 1;
			incl2[2] = 1;
	    }
	    if(id%2==1){
			ent.push_back(entry4);
			ent2.push_back(entry2_4);
			incl[3] = 1;
			incl2[3] = 1;
			
			ent.push_back(entry5);
			ent2.push_back(entry2_5);
			incl[4] = 1;
			incl2[4] = 1;
			
			ent.push_back(entry6);
			//ent.push_back(entry_thread1);
			ent2.push_back(entry2_6);
			incl[5] = 1;
			incl2[5] = 1;
	    }
	    ent.push_back(entry7);
	    ent2.push_back(entry2_7);
	    incl[6] = 1;
		incl2[6] = 1;

	    ent.push_back(entry8);
	    ent2.push_back(entry2_8);
	    incl[7] = 1;
		incl2[7] = 1;

	    ent.push_back(entry9);
	    ent2.push_back(entry2_9);
	    incl[8] = 1;
		incl2[8] = 1;

	    ent.push_back(entry10);
	    ent2.push_back(entry2_10);
	    incl[9] = 1;
		incl2[9] = 1;

        
		bool ran, ran2;

		for(int j=0;j<loops;j++){
		
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
			
			
			ran = CASN(ent);
			ran_shared[j*threadcount+id] = ran;
			int i=0;

			bool non_sharing_thread_ran;
			bool any_thread_ran;
			for(int k=0;k<threadcount-2;k++){
			if(ran_shared[j*threadcount+2+k]==1) non_sharing_thread_ran = true;}
			for(int k=0;k<threadcount;k++){
			if(ran_shared[j*threadcount+k]==1) any_thread_ran = true;}
			#pragma omp barrier
			for(int k=0;k<exp_values.size();k++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if((ran && *ent[i].addr!=ent[i].new1) || (!ran && *ent[i].addr != exp_values[k] && !any_thread_ran) && incl[k]==1 && (!non_sharing_thread_ran || i>3) && abs(*ent[i].addr)<100) 
			{
				if((ran && *ent[i].addr!=ent[i].new1) || (!ran && *ent[i].addr != exp_values[k] && !any_thread_ran) && incl[k]==1 && (!non_sharing_thread_ran || i>3) && abs(*ent[i].addr)<100) std::cout << "check failed thread " << id << " on entry " << i << " should be " << (ran? ent[i].new1 : exp_values[k]) << " but is " << (*ent[i].addr) << " " << any_thread_ran << " " << ran << std::endl;
			}
			if(incl[k]==1)i++;
			}
			
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

			ran2 = CASN(ent2);
			#pragma omp barrier
			ran_shared2[threadcount*j+id]=ran2;
			i=0;
			
			non_sharing_thread_ran = false;
			any_thread_ran = false;
			for(int k=0;k<threadcount-2;k++){
			if(ran_shared[j*threadcount+2+k]==1) non_sharing_thread_ran = true;}
			for(int k=0;k<threadcount;k++){
			if(ran_shared[j*threadcount+k]==1) any_thread_ran = true;}
			for(int k=0;k<exp_values.size();k++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if(((ran2 && *ent2[i].addr!=ent2[i].new1) || (!ran2 && *ent2[i].addr != exp_values[k] && !any_thread_ran)) && incl2[k] == 1 && (!non_sharing_thread_ran || i>3) && abs(*ent2[i].addr)<100) std::cout << "check failed operation 2 thread " << id << " on entry " << i << " should be " << (ran2? ent2[i].new1 : exp_values[k]) << " but is " << (*ent2[i].addr) << std::endl;
			if(incl[k] == 1) i++;
			}
			if(j==0) ato3 = 4*3;
			if(ran) std::cout << " 1" << id;
			if(ran2) std::cout << " 2" << id;
		}
		#pragma omp barrier
		if(id==0) std::cout << std::endl;
	
		
		
		
		// starting failure timing runs
		std::vector<int> timings_first(1000,0), timings_last(1000,0);
		
		std::atomic<word_t> succ0_a{3*4}, succ1_a{3*4}, succ2_a{3*4}, succ3_a{3*4}, succ4_a{3*4}, succ5_a{3*4}, succ6_a{3*4}, succ7_a{3*4}, succ8_a{3*4};
		CASN_entry succ0{&succ0_a,3*4,4*4}, succ1{&succ1_a,3*4,4*4}, succ2{&succ2_a,3*4,4*4}, succ3{&succ3_a,3*4,4*4}, succ4{&succ4_a,3*4,4*4}, succ5{&succ5_a,3*4,4*4}, succ6{&succ6_a,3*4,4*4}, succ7{&succ7_a,3*4,4*4}, succ8{&succ8_a,3*4,4*4}, fail{&fail_a,4*4,3*4};
		std::vector<CASN_entry> fail_first, fail_last;
		
		fail_first.insert(fail_first.end(), {fail, succ0, succ1, succ2, succ3, succ4, succ5, succ6, succ7, succ8});
		fail_last.insert(fail_last.end(), {succ0, succ1, succ2, succ3, succ4, succ5, succ6, succ7, succ8, fail});
		
		
		succ0_a = 3*4;
		succ1_a = 3*4;
		succ2_a = 3*4;
		succ3_a = 3*4;
		succ4_a = 3*4;
		succ5_a = 3*4;
		succ6_a = 3*4;
		succ7_a = 3*4;
		succ8_a = 3*4;
		fail_a = 3*4;
		for(int i=0;i<1000;i++){
		auto start = std::chrono::high_resolution_clock::now();
		ran = CASN(fail_first);
		auto stop = std::chrono::high_resolution_clock::now();
		for(int j=0;j<fail_first.size();j++){
		if(*fail_first[j].addr!=3*4) std::cout << "failure first" << std::endl;
		}
		timings_first[i] = (std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count());
		if(ran!=0) std::cout << "failure" << std::endl;
		//std::cout << "fail first resulted in " << (ran? ("success ") : ("failure ")) << "with runtime in ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count() << std::endl;

		start = std::chrono::high_resolution_clock::now();
		ran = CASN(fail_last);
		stop = std::chrono::high_resolution_clock::now();
		for(int j=0;j<fail_last.size();j++){
		if(*fail_last[j].addr!=3*4) std::cout << "failure last" << std::endl;
		}
		//std::cout << "fail last resulted in " << (ran? ("success ") : ("failure ")) << "with runtime in ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count() << std::endl;
		timings_last[i]=(std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count());
		if(ran!=0) std::cout << "failure" << std::endl;
		}
		std::sort(timings_first.begin(), timings_first.end());
		std::sort(timings_last.begin(), timings_last.end());
		
		//timings_global[id] = timings_first[499];
		//timings_global[id+threadcount] = timings_last[499];
		
		
		
		//starting successful runs
		
		
		CASN_entry shared_0{&succ9_a,3*4,3*4}, succ0_b{&succ0_a,4*4,3*4}, succ1_b{&succ1_a,4*4,3*4}, succ2_b{&succ2_a,4*4,3*4}, succ3_b{&succ3_a,4*4,3*4}, succ4_b{&succ4_a,4*4,3*4}, succ5_b{&succ5_a,4*4,3*4}, succ6_b{&succ6_a,4*4,3*4}, succ7_b{&succ7_a,4*4,3*4}, succ8_b{&succ8_a,4*4,3*4}, succ9{&succ9_a,3*4,3*4}, succ9_b{&succ9_a, 3*4,3*4};
		ato = 3*4;
		ato2 = 3*4;
		ato3 = 3*4;
		ato4 = 3*4;
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
		

		
		std::vector<CASN_entry> succ_entries, succ_entries2;
		succ_entries.insert(succ_entries.end(), {succ9, succ0, succ1, succ2, succ3, succ4, succ5, succ6, succ7, succ8});
		succ_entries2.insert(succ_entries2.end(), {succ9_b, succ0_b, succ1_b, succ2_b, succ3_b, succ4_b, succ5_b, succ6_b, succ7_b, succ8_b});
		
		for(int i=0;i<1000;i++){
				auto start = std::chrono::high_resolution_clock::now();
				bool ran = CASN(succ_entries);
				bool ran2 = CASN(succ_entries2);
				auto stop = std::chrono::high_resolution_clock::now();
				if(!ran || !ran2) std::cout << "failure to run " << ran << " " << ran2 << std::endl;
				timings_first[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count()/2;
		}
		std::sort(timings_first.begin(), timings_first.end());
		timings_global[id+2*threadcount] = timings_first[499];
		
		std::sort(timings_first.begin(), timings_first.end());
		std::sort(timings_last.begin(), timings_last.end());
		int sum=0;
		std::for_each(timings_first.begin(), timings_first.end(), [&] (int n) {sum += n;});
		timings_global[id] = timings_first[0];
		timings_global[id+threadcount] = timings_first[499];
		timings_global[id+2*threadcount] = sum/timings_first.size();
		timings_global[id+3*threadcount] = timings_first[999];
		
		
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
			break;
			}
			output++;
		}
		timings_global[4*threadcount+id] = output;
		
		int a{3}, b{4};
		output=0;
		std::atomic<word_t>temp{3};
		start = std::chrono::high_resolution_clock::now();
		while(std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - start).count() < 3000) {
			CAS1(temp,a,b);
			CAS1(temp,b,a);
			output++;
		}
		timings_global[5*threadcount+id] = output/10;
		
		
    }
    
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
	}
	std::ofstream outputfile;
	outputfile.open("output" + std::to_string(threadcount) +".txt");
	outputfile << "minimum,median,average,maximum,throughput,throughput_base" << std::endl;
	for(int i=0;i<threadcount;i++){
	outputfile << timings_global[i] << "," << timings_global[threadcount + i] << "," << timings_global[2*threadcount +i] << "," << timings_global[3*threadcount +i] << "," << timings_global[4*threadcount +i] << "," << timings_global[5*threadcount +i] << std::endl;
	}
	outputfile.close();
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}
