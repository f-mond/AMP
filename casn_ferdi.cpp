#include<vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "casn2.cpp" 
#include <chrono>

int main() {
    std::atomic<word_t> ato{4},ato2{4*3},ato3{3}, ato4{4*3}, ato5{4*3}, ato6{4*3}, ato7{4*3}, ato8{4*3}, fail_a{3}, succ9_a{3};
	CASN_entry entry1{&ato, 4, 6}, entry2{&ato2,4*3,4*6}, entry3{&ato3,4*3,4*6}, entry4{&ato4,4*3,4*6}, entry5{&ato5,4*3,4*6}, entry6{&ato6,4*3,4*6}, entry7{&ato7,4*3,4*4}, entry8{&ato8,4*3,4*3};
	CASN_entry entry2_0{&ato, 6, 4}, entry2_2{&ato2, 4*6,4*3}, entry2_3{&ato3, 4*6,3*4}, entry2_4{&ato4, 4*6,4*3}, entry2_5{&ato5, 4*6,4*3}, entry2_6{&ato6, 4*6,4*3}, entry2_7{&ato7, 4*4,4*3}, entry2_8{&ato8, 4*3,4*3};
    
    CASN_entry entry_thread1{&ato3,3,4*6}, entry_thread2{&ato3,3,4*6};
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;
    std::vector<int> casn_throughput;
    std::vector<int> cas_throughput;


/*    std::vector<CASN_entry> N_entries;
    N_entries.push_back(entry1);    
	std::cout << ato << std::endl;
	
	CASN(N_entries);
	//CASN_desc *d = new CASN_desc(UNDECIDED, N_entries);
	//std::cout << std::hex << CAS1(*d->entry[0].addr, 6, 3) << std::endl;
	std::cout << std::endl << ato << std::endl;


*/
	int threadcount = 6;
    omp_set_dynamic(0);
    omp_set_num_threads(threadcount);
    int loops = 10;
    std::vector<int> ran_shared(loops*threadcount,0);
    std::vector<int> ran_shared2(loops*threadcount,0);
    std::vector<int> timings_global(threadcount*3,0);
  
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
			#pragma omp barrier
			bool non_sharing_thread_ran;
			bool any_thread_ran;
			for(int k=0;k<threadcount-2;k++){
			if(ran_shared[j*threadcount+2+k]==1) non_sharing_thread_ran = true;}
			for(int k=0;k<threadcount;k++){
			if(ran_shared[j*threadcount+k]==1) any_thread_ran = true;}
			
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
			//std::cout << id << " " << ran << " " << ran2 << std::endl;
/*			if(id==0){
				std::cout << "current ";
				for(int i=0;i<old_values.size();i++){std::cout << *ent[i].addr << " ";}
				std::cout << std::endl << "expected ";
				for(int i=0;i<old_values.size();i++){std::cout << ent2[i].exp1 << " ";}
				std::cout << std::endl << "old ";
				for(int i=0;i<old_values.size();i++){std::cout << old_values[i] << " ";}
				std::cout << std::endl;
			}
*/		}
		//std::cout << std::endl;
		
		
		
		// starting failure timing runs
		std::atomic<word_t> succ0_a{3}, succ1_a{3}, succ2_a{3}, succ3_a{3}, succ4_a{3}, succ5_a{3}, succ6_a{3}, succ7_a{3}, succ8_a{3};
		CASN_entry succ0{&succ0_a,3,4}, succ1{&succ1_a,3,4}, succ2{&succ2_a,3,4}, succ3{&succ3_a,3,4}, succ4{&succ4_a,3,4}, succ5{&succ5_a,3,4}, succ6{&succ6_a,3,4}, succ7{&succ7_a,3,4}, succ8{&succ8_a,3,4}, fail{&fail_a,4,3};
		std::vector<CASN_entry> fail_first, fail_last;
		std::vector<int> timings_first, timings_last;
		fail_first.push_back(fail);
		fail_first.push_back(succ0);
		fail_last.push_back(succ0);
		fail_first.push_back(succ1);
		fail_last.push_back(succ1);
		fail_first.push_back(succ2);
		fail_last.push_back(succ2);
		fail_first.push_back(succ3);
		fail_last.push_back(succ3);
		fail_first.push_back(succ4);
		fail_last.push_back(succ4);
		fail_first.push_back(succ5);
		fail_last.push_back(succ5);
		fail_first.push_back(succ6);
		fail_last.push_back(succ6);
		fail_first.push_back(succ7);
		fail_last.push_back(succ7);
		fail_first.push_back(succ8);
		fail_last.push_back(succ8);
		fail_last.push_back(fail);
		
		for(int i=0;i<1000;i++){
		auto start = std::chrono::high_resolution_clock::now();
		ran = CASN(fail_first);
		auto stop = std::chrono::high_resolution_clock::now();
		for(int j=0;j<fail_first.size();j++){
		if(*fail_first[j].addr!=3) std::cout << "failure" << std::endl;
		}
		timings_first.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count());
		if(ran!=0) std::cout << "failure" << std::endl;
		//std::cout << "fail first resulted in " << (ran? ("success ") : ("failure ")) << "with runtime in ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count() << std::endl;
			
		start = std::chrono::high_resolution_clock::now();
		ran = CASN(fail_last);
		stop = std::chrono::high_resolution_clock::now();
		for(int j=0;j<fail_last.size();j++){
		if(*fail_last[j].addr!=3) std::cout << "failure" << std::endl;
		}
		//std::cout << "fail last resulted in " << (ran? ("success ") : ("failure ")) << "with runtime in ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count() << std::endl;
		timings_last.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count());
		if(ran!=0) std::cout << "failure" << std::endl;
		}
		std::sort(timings_first.begin(), timings_first.end());
		std::sort(timings_last.begin(), timings_last.end());
		timings_global[id] = timings_first[499];
		timings_global[id+threadcount] = timings_last[499];
		if(id==0){
		int sum;
		std::for_each(timings_first.begin(), timings_first.end(), [&] (int n) {sum += n;});
		std::cout << "minimum execution time for fail on first entry in ns: " << timings_first[0] << std::endl;
		std::cout << "median execution time for fail on first entry in ns: " << timings_first[499] << std::endl;
		std::cout << "average execution time for fail on first entry in ns: " << sum/timings_first.size() << std::endl;
		std::cout << "maximum execution time for fail on first entry in ns: " << timings_first[999] << std::endl << std::endl;
		std::cout << "minimum execution time for fail on last entry in ns: " << timings_last[0] << std::endl;
		std::cout << "median execution time for fail on last entry in ns: " << timings_last[499] << std::endl;
		sum = 0;
		std::for_each(timings_last.begin(), timings_last.end(), [&] (int n) {sum+=n;});
		std::cout << "average execution time for fail on last entry in ns: " << sum/timings_last.size() << std::endl;
		std::cout << "maximum execution time for fail on last entry in ns: " << timings_last[999] << std::endl;}
		
		
		
		//starting successful runs
		
//		std::atomic<word_t> succ0_b{3}, succ1_b{3}, succ2_b{3}, succ3_b{3}, succ4_b{3}, succ5_b{3}, succ6_b{3}, succ7_b{3}, succ8_b{3}, succ9_b{3};
		CASN_entry succ9{&succ9_a,3,3}, succ0_b{&succ0_a,4,3}, succ1_b{&succ1_a,4,3}, succ2_b{&succ2_a,4,3}, succ3_b{&succ3_a,4,3}, succ4_b{&succ4_a,4,3}, succ5_b{&succ5_a,4,3}, succ6_b{&succ6_a,4,3}, succ7_b{&succ7_a,4,3}, succ8_b{&succ8_a,4,3}, succ9_b{&succ9_a,3,3};
		
		std::vector<CASN_entry> succ_entries, succ_entries2;
		succ_entries.push_back(succ0);
		succ_entries2.push_back(succ0_b);
		succ_entries.push_back(succ1);
		succ_entries2.push_back(succ1_b);
		succ_entries.push_back(succ2);
		succ_entries2.push_back(succ2_b);
		succ_entries.push_back(succ3);
		succ_entries2.push_back(succ3_b);
		succ_entries.push_back(succ4);
		succ_entries2.push_back(succ4_b);
		succ_entries.push_back(succ5);
		succ_entries2.push_back(succ5_b);
		succ_entries.push_back(succ6);
		succ_entries2.push_back(succ6_b);
		succ_entries.push_back(succ7);
		succ_entries2.push_back(succ7_b);
		succ_entries.push_back(succ8);
		succ_entries2.push_back(succ8_b);
		succ_entries.push_back(succ9);
		succ_entries2.push_back(succ9_b);
		
		for(int i=0;i<1000;i++){
			auto start = std::chrono::high_resolution_clock::now();
			bool ran = CASN(succ_entries);
			bool ran2 = CASN(succ_entries2);
			auto stop = std::chrono::high_resolution_clock::now();
			if(!ran || !ran2) std::cout << "failure to run" << std::endl;
			timings_first[i] = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count()/2;
		}
		CASN(succ_entries);
		#pragma omp barrier
		if(succ0_a !=4 || succ1_a !=4 ||succ2_a !=4 ||succ3_a !=4 ||succ4_a !=4 ||succ5_a !=4 ||succ6_a !=4 ||succ7_a !=4 ||succ8_a !=4 ||succ9_a !=3) {std::cout << "failure to get right result " << id << std::endl;
		std::cout << succ0_a << " " << succ1_a << " " << succ2_a << " " << succ3_a << " " << succ4_a << " " << succ5_a << " " << succ6_a << " " << succ7_a << " " << succ8_a << " " << succ9_a << " " << std::endl;}
		std::sort(timings_first.begin(), timings_first.end());
		timings_global[id+2*threadcount] = timings_first[499];
		if(id==0){
		std::sort(timings_first.begin(), timings_first.end());
		std::sort(timings_last.begin(), timings_last.end());
		int sum=0;
		std::for_each(timings_first.begin(), timings_first.end(), [&] (int n) {sum += n;});
		std::cout << "minimum execution time for 10 CAS success in ns: " << timings_first[0] << std::endl;
		std::cout << "median execution time for 10 CAS success in ns: " << timings_first[499] << std::endl;
		std::cout << "average execution time for 10 CAS success in ns: " << sum/timings_first.size() << std::endl;
		std::cout << "maximum execution time for 10 CAS success in ns: " << timings_first[999] << std::endl << std::endl;
		}
    }
    //std::cout << ato << std::endl;
/**/
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}
