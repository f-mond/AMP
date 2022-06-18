#include<vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "casn2.cpp" 

int main() {
    std::atomic<word_t> ato{4},ato2{4*3},ato3{3}, ato4{4*3}, ato5{4*3}, ato6{4*3}, ato7{4*3}, ato8{4*3};
	CASN_entry entry1{&ato, 4, 6}, entry2{&ato2,4*3,4*6}, entry3{&ato3,4*3,4*6}, entry4{&ato4,4*3,4*6}, entry5{&ato5,4*3,4*6}, entry6{&ato6,4*3,4*6}, entry7{&ato7,4*3,4*4}, entry8{&ato8,4*3,4*3};
	CASN_entry entry2_0{&ato, 6, 4}, entry2_2{&ato2, 4*6,4*3}, entry2_3{&ato3, 4*6,4*3}, entry2_4{&ato4, 4*6,4*3}, entry2_5{&ato5, 4*6,4*3}, entry2_6{&ato6, 4*6,4*3}, entry2_7{&ato7, 4*4,4*3}, entry2_8{&ato8, 4*3,4*3};
    
    CASN_entry entry_thread1{&ato3,3,4*6}, entry_thread2{&ato3,4*6,4*3};
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
    omp_set_dynamic(0);
    omp_set_num_threads(2);
    std::vector<int> ran_shared(6,0);
    std::vector<int> ran_shared2(6,0);
  
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        std::atomic<word_t> ato9{4*4}, ato10{4*4};
	    CASN_entry entry9{&ato9,4*4,4*5}, entry10{&ato10,4*4,4*5};
	    CASN_entry entry2_9{&ato9, 4*5,4*4}, entry2_10{&ato10,4*5,4*4};
	    std::vector<CASN_entry> ent;
	    std::vector<CASN_entry> ent2;
	    std::vector<int> old_values;
	    std::vector<bool> shared;

        if(id%2==0){
			ent.push_back(entry1);
			ent2.push_back(entry2_0);
			old_values.push_back(*entry1.addr);
			shared.push_back(false);

			ent.push_back(entry2);
			ent2.push_back(entry2_2);
			old_values.push_back(*entry2.addr);
			shared.push_back(false);

			//ent.push_back(entry3);
			ent.push_back(entry_thread1);
			ent2.push_back(entry2_3);
			old_values.push_back(*entry3.addr);
			shared.push_back(false);
	    }
	    if(id%2==1){
			ent.push_back(entry4);
			ent2.push_back(entry2_4);
			shared.push_back(false);
			old_values.push_back(*entry4.addr);
			ent.push_back(entry5);
			ent2.push_back(entry2_5);
			shared.push_back(false);
			old_values.push_back(*entry5.addr);
			//ent.push_back(entry6);
			ent.push_back(entry_thread2);
			ent2.push_back(entry2_6);
			shared.push_back(false);
			//old_values.push_back(*entry6.addr);
			old_values.push_back(*entry_thread2.addr);
	    }
	    ent.push_back(entry7);
	    ent2.push_back(entry2_7);
	    shared.push_back(true);
	    old_values.push_back(*entry7.addr);

	    ent.push_back(entry8);
	    ent2.push_back(entry2_8);
	    shared.push_back(true);
	    old_values.push_back(*entry8.addr);

	    ent.push_back(entry9);
	    ent2.push_back(entry2_9);
	    shared.push_back(true);
	    old_values.push_back(*entry9.addr);

	    ent.push_back(entry10);
	    ent2.push_back(entry2_10);
	    shared.push_back(true);
	    old_values.push_back(*entry10.addr);

        
		bool ran, ran2;

		for(int j=0;j<3;j++){
		
		
/*			std::cout << "current ";
			for(int i=0;i<old_values.size();i++){std::cout << *ent[i].addr << " ";}
			std::cout << std::endl << "expected ";
			for(int i=0;i<old_values.size();i++){std::cout << ent[i].exp1 << " ";}
*/			std::cout << std::endl << "old ";
			for(int i=0;i<old_values.size();i++){std::cout << old_values[i] << " ";}
//			std::cout << std::endl << "post run ";
	
			
			ran = CASN(ent);
			ran_shared[j*2+id] = ran;
//			std::cout << "CASN 1 done" << std::endl;
			for(int i=0;i<old_values.size();i++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if((ran && *ent[i].addr!=ent[i].new1) || (!ran && *ent[i].addr != old_values[i]) /*&& shared[i]==false*/) std::cout << "check failed thread " << id << " on entry " << i << " should be " << (ran? ent[i].new1 : old_values[i]) << " but is " << (*ent[i].addr) << " " << ran << std::endl;
			old_values[i] = *ent[i].addr;
		}
			
/*
			for(int i=0;i<old_values.size();i++){std::cout << *ent[i].addr << " ";}
			std::cout << std::endl << "CAS1 " << ran << std::endl << std::endl;
			std::cout << "current ";
			for(int i=0;i<old_values.size();i++){std::cout << *ent2[i].addr << " ";}
			std::cout << std::endl << "expected ";
			for(int i=0;i<old_values.size();i++){std::cout << ent2[i].exp1 << " ";}
			std::cout << std::endl << "new ";
			for(int i=0;i<old_values.size();i++){std::cout << ent2[i].new1 << " ";}
			std::cout << std::endl << "post run ";
*/
			
			ran2 = CASN(ent2);
			ran_shared2[2*j+id]=ran2;
//			for(int i=0;i<old_values.size();i++){std::cout << *ent2[i].addr << " ";}
//			std::cout << std::endl << "CAS2 " << ran2 << std::endl << std::endl;
			for(int i=0;i<old_values.size();i++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if((ran2 && *ent2[i].addr!=ent2[i].new1) || (!ran2 && *ent2[i].addr != old_values[i]) /*&& shared[i]==false*/) std::cout << "check failed operation 2 thread " << id << " on entry " << i << " should be " << (ran? ent[i].new1 : old_values[i]) << " but is " << (*ent[i].addr) << " " << ran << std::endl;
			old_values[i] = *ent[i].addr;
			}
			std::cout << id << " " << ran << " " << ran2 << std::endl;
			//ato3 = 4*3;
			//old_values[2] = 4*3;
			//ato = 6;
			//old_values[0] = 6;
		}

    }
    //std::cout << ato << std::endl;
/**/
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}
