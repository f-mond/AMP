#include<vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "casn2.cpp"

int main() {
    std::atomic<word_t> ato{3},ato2{3},ato3{3}, ato4{3}, ato5{3}, ato6{3}, ato7{3}, ato8{3};
	CASN_entry entry1{&ato, 3, 5}, entry2{&ato2,3,5}, entry3{&ato3,3,5}, entry4{&ato4,3,5}, entry5{&ato5,3,5}, entry6{&ato6,3,5}, entry7{&ato7,3,4}, entry8{&ato8,3,3};
	CASN_entry entry2_0{&ato, 5, 3}, entry2_2{&ato2, 5,3}, entry2_3{&ato3, 5,3}, entry2_4{&ato4, 5,3}, entry2_5{&ato5, 5,3}, entry2_6{&ato6, 5,3}, entry2_7{&ato7, 4,3}, entry2_8{&ato8, 3,3};
    
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;
    std::vector<int> casn_throughput;
    std::vector<int> cas_throughput;

  
    std::vector<CASN_entry> N_entries;
    N_entries.push_back(entry1);    
    N_entries.push_back(entry2);
    N_entries.push_back(entry3);

    std::vector<CASN_entry> N_entries2;
    N_entries2.push_back(entry2_0);    
    N_entries2.push_back(entry2_2);
    N_entries2.push_back(entry2_3);

    std::cout << ato;
    std::cout << ato2;
    std::cout << ato3;
    std::cout << CASN(N_entries) << std::endl;
    std::cout << ato;
    std::cout << ato2;
    std::cout << ato3;
    std::cout << CASN(N_entries2) << std::endl;
    std::cout << ato;
    std::cout << ato2;
    std::cout << ato3;
    /*
    omp_set_dynamic(0);
    omp_set_num_threads(1);
    
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        std::atomic<word_t> ato9{4}, ato10{4};
	    CASN_entry entry9{&ato9,4,5}, entry10{&ato10,4,5};
	    CASN_entry entry2_9{&ato9, 5,4}, entry2_10{&ato10,5,4};
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
			//std::cout << ran << " " << ran2 << std::endl;
		}

    }
    
    */
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}