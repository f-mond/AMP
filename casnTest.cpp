#include<vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "casn2.cpp"

int main() {
    std::atomic<word_t> ato{5},ato2{3},ato3{3}, ato4{3}, ato5{3}, ato6{3}, ato7{3}, ato8{3};
	CASN_entry entry1{&ato, 5, 6}, entry2{&ato2,3,6}, entry3{&ato3,3,6}, entry4{&ato4,3,6}, entry5{&ato5,3,6}, entry6{&ato6,3,6}, entry7{&ato7,3,4}, entry8{&ato8,3,3};
	CASN_entry entry2_0{&ato, 6, 3}, entry2_2{&ato2, 6,3}, entry2_3{&ato3, 6,3}, entry2_4{&ato4, 6,3}, entry2_5{&ato5, 6,3}, entry2_6{&ato6, 6,3}, entry2_7{&ato7, 4,3}, entry2_8{&ato8, 3,3};
    
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;
    std::vector<int> casn_throughput;
    std::vector<int> cas_throughput;


    std::vector<CASN_entry> N_entries;
    N_entries.push_back(entry1);    
   
 
    omp_set_dynamic(0);
    omp_set_num_threads(1);
   
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        std::atomic<word_t> ato9{4}, ato10{4};
	    CASN_entry entry9{&ato9,4,6}, entry10{&ato10,4,6};
	    CASN_entry entry2_9{&ato9, 6,4}, entry2_10{&ato10,6,4};
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

			ent.push_back(entry3);
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
			ent.push_back(entry6);
			ent2.push_back(entry2_6);
			shared.push_back(false);
			old_values.push_back(*entry6.addr);
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

		for(int i=0;i<10;i++){
			ran = CASN(ent);
			
			ran2 = CASN(ent2);
			std::cout << ran << " " << ran2 << std::endl;
		}

    }
    
/**/
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}