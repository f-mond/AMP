#include<vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "casn2.cpp" 

int main() {
    std::atomic<word_t> ato{4*4},ato2{4*3},ato3{4*3}, ato4{4*3}, ato5{4*3}, ato6{4*3}, ato7{4*3}, ato8{4*3};
	CASN_entry entry1{&ato, 4*4, 4*6}, entry2{&ato2, 4*3, 4*6}, entry3{&ato3,4*3,4*6}, entry4{&ato4,4*3,4*6}, entry5{&ato5,4*3,4*6}, entry6{&ato6,4*3,4*6}, entry7{&ato7,4*3,4*4}, entry8{&ato8,4*3,4*3};
	CASN_entry entry2_0{&ato, 4*6, 4*4}, entry2_2{&ato2, 4*6,4*3}, entry2_3{&ato3, 4*6,4*3}, entry2_4{&ato4, 4*6,4*3}, entry2_5{&ato5, 4*6,4*3}, entry2_6{&ato6, 4*6,4*3}, entry2_7{&ato7, 4*4,4*3}, entry2_8{&ato8, 4*3,4*3};
    
    CASN_entry entry_thread1{&ato3,4*3,4*6}, entry_thread2{&ato3, 4*6, 4*3};
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
	int threadcount = 2;
    omp_set_dynamic(0);
    omp_set_num_threads(threadcount);
    int loops = 3;
    std::vector<int> ran_shared(loops*threadcount,0);
    std::vector<int> ran_shared2(loops*threadcount,0);
  
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        std::atomic<word_t> ato9{4*4}, ato10{4*4};
	    CASN_entry entry9{&ato9,4*4,4*5}, entry10{&ato10,4*4,4*5};
	    CASN_entry entry2_9{&ato9, 4*5,4*4}, entry2_10{&ato10,4*5,4*4};
	    std::vector<CASN_entry> ent;
	    std::vector<CASN_entry> ent2;
	    std::vector<int> old_values(10,0);
	    std::vector<int> exp_values(10,0);
	    std::vector<bool> incl(10,0);
	    std::vector<bool> incl2(10,0);
        if(id%2==0){
			ent.push_back(entry1);
			ent2.push_back(entry2_0);
			incl[0] = true;
			incl2[0] = 1;

			ent.push_back(entry2);
			ent2.push_back(entry2_2);
			incl[1] = 1;
			incl2[1] = 1;

			//ent.push_back(entry3);
			ent.push_back(entry_thread2);
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
			
			//ent.push_back(entry6);
			ent.push_back(entry_thread1);
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

		for(int j=0;j<3;j++){
		

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
			ran_shared[j*2+id] = ran;
			int i=0;
			for(int k=0;k<exp_values.size();k++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			
			if((ran && *ent[i].addr!=ent[i].new1) || (!ran && *ent[i].addr != exp_values[k] && ran_shared[j*2+abs(id-1)] != true) && incl[k]==1) std::cout << "check failed thread " << id << " on entry " << i << " should be " << (ran? ent[i].new1 : exp_values[k]) << " but is " << (*ent[i].addr) << std::endl;
			if(incl[k]==1)i++;
			}
			
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
			ran_shared2[2*j+id]=ran2;
			i=0;
			for(int k=0;k<exp_values.size();k++){
//			checking if the values were reset in case of fail, or if they got changed in case of success
			if((ran2 && *ent2[i].addr!=ent2[i].new1) || (!ran2 && *ent2[i].addr != exp_values[k] && ran_shared2[j*2+abs(id-1)] !=true) && incl2[k] == 1) std::cout << "check failed operation 2 thread " << id << " on entry " << i << " should be " << (ran2? ent2[i].new1 : exp_values[k]) << " but is " << (*ent2[i].addr) << std::endl;
			if(incl[k] == 1) i++;
			}
			
			std::cout << id << " " << ran << " " << ran2 << std::endl;
			//ato3 = 4*3;
			//old_values[2] = 4*3;
			//ato = 6;
			//old_values[0] = 6;
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

    }
    //std::cout << ato << std::endl;
/**/
	std::cout << std::endl << "finished execution" << std::endl;
    
    return 0;
}
