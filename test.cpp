#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <omp.h>
#include <climits>
#include <random>
#include <algorithm>
#include <chrono>

#define UNDECIDED 0
#define SUCCEEDED 1
#define FAILED 2
#define CASNDESC 3
#define RDCSSDESC 2

class Zeugs {
    public:
        int v1;
        double v2;
        std::string s1;

    Zeugs(int _v1, double _v2, std::string _s1) {
        v1=_v1;
        v2=_v2;
        s1=_s1;
    }
};

class RDCSSDescriptor {
    public:
    std::atomic<long int> *a1;
    const long int o1{7};
    std::atomic<long int> *a2;
    const long int o2{3};
    const long int n2{4};
    
    RDCSSDescriptor(std::atomic<long int> *_a1, std::atomic<long int> *_a2, long int _o1, long int _o2, long int _n2) : o1(_o1) , o2(_o2), n2(_n2), a1(_a1), a2(_a2){    }
};


struct entries {
    std::atomic<long int> *addr;
    std::atomic<long int> tmp{0};
    long int oldVal;
    long int newVal;
    entries() {
    	addr = &tmp;
    	oldVal=0;
    	newVal=0;
    }
    entries(std::atomic<long int> *add, long int old, long int ne) : addr(add), oldVal(old), newVal(ne) {}
    entries(const entries& cp){
    	addr=cp.addr;
    	oldVal = cp.oldVal;
    	newVal = cp.newVal;
	}
};

class CASNDescriptor {
    public:
    std::atomic<long int> status;
    int count; 
    std::vector<entries> entry;

    void resize(int size){
        count=size;
		entry.resize(size);		
	};
	CASNDescriptor(std::vector<entries> initial_entries) : entry(initial_entries), count(initial_entries.size()) , status(UNDECIDED){}

};

int IsDescriptor(void *p1) {
    //void *p1 = &r; // the pointer we want to store the value into
    //std::cout << p1 << std::endl;
    int tag = (uintptr_t)p1 & 0x03;
    return tag;
}

//test
int IsDescriptorCASN(void *p1) {
    //void *p1 = &r; // the pointer we want to store the value into
    //std::cout << p1 << std::endl;
    int tag = (uintptr_t)p1 & 0x06;
    return tag;
}
//test
void* SetTagCASN(void *p1) {
    int tag=3;
    const uintptr_t MASK = ~0x04ULL;
    long int *p2=(long int *)(((uintptr_t)p1 & MASK) | tag);
    //std::cout << "set " << p1 << " " << p2 << std::endl;
    return p2;
}
//test
void* GetDescriptor(void *p2) {
    const uintptr_t MASK = ~0x07ULL;
    intptr_t p3 = (uintptr_t)p2 & MASK;
    return (CASNDescriptor*)p3;
}

void* SetTagRDCSS(void *p1) {
    int tag=2;
    const uintptr_t MASK = ~0x03ULL;
    long int *p2=(long int *)(((uintptr_t)p1 & MASK) | tag);
    //std::cout << "set " << p1 << " " << p2 << std::endl;
    return p2;
}

/*
RDCSSDescriptor GetDescriptorRDCSS(void *p2) {
    const uintptr_t MASK = ~0x03ULL;
    intptr_t p3 = (uintptr_t)p2 & MASK;
    return *(RDCSSDescriptor*)p3;
}
*/

CASNDescriptor* GetDescriptorCASN(void *p2) {
    const uintptr_t MASK = ~0x03ULL;
    intptr_t p3 = (uintptr_t)p2 & MASK;
    return (CASNDescriptor*)p3;
}

long int CAS1(std::atomic<long int> &a, long int o, long int n) {
    long int old=a;
    //std::cout << "doing CAS on value " << a << " with expected value " << o << " and new value " << n << std::endl;
    a.compare_exchange_weak(o, n);
    //std::cout << "CAS resulted in " << a << std::endl;
    return old;
}

void Complete(RDCSSDescriptor *d) {
	//the seg faults happen somewhere in here when a thread picks up a rdcss descriptor and tries to complete
	//std::cout << "set v" << std::endl;
	long int v = *d->a1;
	//std::cout << "v set as " << v << std::endl;
	if(v==d->o1) {
	//std::cout << "attempting second CAS1 with a2 " << *d->a2 << " expected value " << (long int)SetTag(d) << " new value " << d->n2 << std::endl;
	//std::cout << "CAS1" << std::endl;
	CAS1(*d->a2,(long int)SetTagRDCSS(d),d->n2);
	//std::cout << "second CAS1 finished with a2 now " << *d->a2 << std::endl;
	}
	else {
	//std::cout << "CAS1" << std::endl;
	CAS1(*d->a2,(long int)SetTagRDCSS(d),d->o2);
	//std::cout << "second CAS1 did not succeed, undoing that CAS1" << std::endl;
	}
}

long int RDCSS (RDCSSDescriptor *d, int id) {
    long int r;
    //std::cout << "RDCSS Descriptor " << d << std::endl;
    do {
        r=CAS1(*d->a2, d->o2, (long int)SetTagRDCSS(d));
        //std::cout << "thread " << id << " first CAS happened, value " << r << " returned, we now have value " << *d->a2 << " at the data location" << std::endl;
        if(IsDescriptor((void*)r)==RDCSSDESC && r>INT_MAX) {
        std::cout << "thread " << id << " we found a descriptor in RDCSS " << r << " and will complete it now " << r << std::endl;
        Complete((RDCSSDescriptor*)GetDescriptorCASN((void*)r));
        }
        //std::cout << "r " << r << std::endl;
    } while(IsDescriptor((void*)r)==RDCSSDESC && r>INT_MAX);
    if(r==d->o2) {
    //std::cout << "thread " << id << " CAS1 was successful and we will now complete the operation" << std::endl;
    Complete(d);
    }
    //std::cout << "thread " << id << " finished RDCSS and have values a1 " << *d->a1 << " a2 " << *d->a2 << std::endl;
    return r;
}

bool CASN(CASNDescriptor *cd,int id =0){
	if(cd->status == UNDECIDED){
		int status = SUCCEEDED;
		//std::cout << "thread " << id << " started and set status to succeeded " << status << std::endl;
		//std::cout << "count " << cd->count << std::endl;
		for(int i=0; (i<cd->count) && (status == SUCCEEDED); i++){
			//std::cout << "thread " << id << " starting loop with i " << i << " count " << cd->count << " status " << status << std::endl;
			retry_entry: auto entry = cd->entry[i];
			RDCSSDescriptor temp = RDCSSDescriptor(&cd->status, entry.addr, UNDECIDED, entry.oldVal, (long int)SetTagCASN(cd));
			long int val = RDCSS(&temp,id);
			//std::cout << "thread " << id << " value returned from RDCSS on entry " << i << " : " << val << " and address set to " << *entry.addr << std::endl;
			if(IsDescriptor((void*)val)==CASNDESC && val> INT_MAX){
				std::cout << "found CASNDescriptor " << val << std::endl;
				if(cd != GetDescriptorCASN((void*)val)){
					std::cout << "thread " << id << " found a descriptor and doing CASN on that one now " << val << std::endl << std::endl;
					CASN(GetDescriptorCASN((void*)val));
					goto retry_entry;
					std::cout << "going to reentry" << std::endl;
				}
			}
			else if(val != entry.oldVal){
				status = FAILED;
				std::cout << "failed to assign values, got value " << val << " instead of value " << entry.oldVal << std::endl;
			}
			//std::cout << "thread " << id << " finished entry " << i << " of " << cd->count << ", having written value " << *cd->entry[i].addr << " to the entry" << std::endl;
		}
		//std::cout << "thread " << id << " finished loop through entries, now assigning success status " << cd->status << std::endl;
		CAS1((cd->status), UNDECIDED, status);
		//std::cout << "final status is " << cd->status << std::endl;
	}
	bool succeeded = (cd->status == SUCCEEDED);
	for(int i=0; i<cd->count; i++){
		//std::cout << "thread " << id << " entry " << i << " has value " << *cd->entry[i].addr << std::endl;
	}
	for(int i=0; i<cd->count; i++){
		CAS1(*(cd->entry[i].addr), (long int)SetTagCASN(cd), succeeded?(cd->entry[i].newVal) : (cd->entry[i].oldVal));
		//std::cout << "thread " << id << " writing value " << (succeeded?(cd->entry[i].newVal) : (cd->entry[i].oldVal)) << " to entry " << i << std::endl;
	}
	return succeeded;
}

long int RDCSSRead(void *addr){
	void *r;
	do {
		r = addr;
		if(IsDescriptor(r)==RDCSSDESC) Complete((RDCSSDescriptor*)r);
	} while(IsDescriptor(r)==RDCSSDESC);
	return (long int)r;
}



int main() {
    /*
    Zeugs val(1, 1.1, "Hi");
    Zeugs val2(0, 0, "");
    void *p1=&val;
    
    std::atomic<long int> test;
    test=7;
    void *p2 = SetTag(&val);
    std::cout << p2 << std::endl;
    long int ups=123;
    
    std::cout << IsDescriptor(p2)<< " " << IsDescriptor(&ups)<< std::endl;

    val2=GetDescriptor(p2);
    std::cout << val2.s1 << std::endl;
    
    std::atomic<long int> a{7};
    CAS1(a, 4, 2);
    std::cout << a << std::endl;
    */
    std::atomic<long int> ato{3},ato2{3},ato3{3}, ato4{3}, ato5{3}, ato6{3}, ato7{3}, ato8{3};
	entries entry{&ato, 3, 5}, entry2{&ato2,3,5}, entry3{&ato3,3,5}, entry4{&ato4,3,5}, entry5{&ato5,3,5}, entry6{&ato6,3,5}, entry7{&ato7,3,3}, entry8{&ato8,3,3};
    omp_set_dynamic(0);
    omp_set_num_threads(2);
    std::vector<int> casn_runtime;
    std::vector<int> cas_runtime;
	#pragma omp parallel shared(ato)
	{
		int id = omp_get_thread_num();
	    
	    std::atomic<long int> ato9{4}, ato10{4};
	    entries entry9{&ato9,4,5}, entry10{&ato10,4,5};
	    std::vector<entries> ent;
	    std::vector<int> old_values;
	    if(id==0){
	    ent.push_back(entry);
	    old_values.push_back(*entry.addr);
	    ent.push_back(entry2);
	    old_values.push_back(*entry2.addr);
	    ent.push_back(entry3);
	    old_values.push_back(*entry3.addr);
	    }
	    if(id==1){
	    ent.push_back(entry4);
	    old_values.push_back(*entry4.addr);
	    ent.push_back(entry5);
	    old_values.push_back(*entry5.addr);
	    ent.push_back(entry6);
	    old_values.push_back(*entry6.addr);
	    }
	    ent.push_back(entry7);
	    old_values.push_back(*entry7.addr);
	    ent.push_back(entry8);
	    old_values.push_back(*entry8.addr);
	    ent.push_back(entry9);
	    old_values.push_back(*entry9.addr);
	    ent.push_back(entry10);
	    old_values.push_back(*entry10.addr);
	    
        CASNDescriptor casndesc(ent);
		std::cout << "starting CASN on thread " << id << " size " << casndesc.count << std::endl;
		auto start = std::chrono::high_resolution_clock::now();
		bool ran = CASN(&casndesc,id);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
		std::cout << "CASN resulted in " << (ran?("succeeded") : ("failed")) << " on thread " << id << std::endl;
		casn_runtime.push_back(duration.count());
		for(int i=0;i<old_values.size();i++){
			//checking if the values were reset in case of fail, or if they got changed in case of success
			if((ran && *ent[i].addr!=ent[i].newVal) || (!ran && *ent[i].addr != old_values[i])) std::cout << "check failed" << std::endl;
		}
		start = std::chrono::high_resolution_clock::now();
		long int tmp = 3;
		for(int i=0;i<old_values.size();i++){
			ato.compare_exchange_weak(tmp,(long int)3);
		}
		stop = std::chrono::high_resolution_clock::now();
		auto duration_base = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
		cas_runtime.push_back(duration_base.count());
	}
	for(int i=0;i<cas_runtime.size();i++){
		std::cout << "thread " << i << " runtime for CASN " << casn_runtime[i] << " runtime for N CAS " << cas_runtime[i] << std::endl;
		}
	/*
    entries entry = {&ato3, 4, 5};
    entries entry2 = {&ato4, 4, 5};
    //std::vector<entries> ent{entry};
    //CASNDescriptor desc(ent);
    RDCSSDescriptor desc(&ato3, &ato4, 4, 4, 5);
    std::cout << &desc << std::endl;
    void* testing = &desc;
	std::cout << IsDescriptor(SetTagCASN(testing)) << std::endl;
	std::cout << IsDescriptor(SetTagRDCSS(testing)) << std::endl;
	std::cout << IsDescriptor(testing) << std::endl;
	std::cout << testing << std::endl;
	std::cout << SetTagCASN(testing) << std::endl;
	std::cout << SetTagRDCSS(testing) << std::endl;
	std::cout << GetDescriptorCASN(testing) << std::endl;
	std::cout << GetDescriptorCASN(SetTagCASN(testing)) << std::endl;
	std::cout << GetDescriptorCASN(SetTagRDCSS(testing)) << std::endl;
	std::cout << GetDescriptor(SetTagCASN(testing)) << std::endl;
	std::cout << GetDescriptor(SetTagRDCSS(testing)) << std::endl;
	
    std::cout << IsDescriptor((void*)14) << std::endl;
    std::cout << ((long int)testing > INT_MAX) << std::endl;
    */

	std::cout << "finished execution" << std::endl;
	
    //RDCSSDescriptor desc(7,4,7,4,1);
    //long int temp;
    //temp = RDCSS(&desc);
    
    return 0;
}
