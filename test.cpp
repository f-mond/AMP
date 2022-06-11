#include <iostream>
#include <string>
#include <atomic>
#include <vector>
//#include <omp.h>

#define UNDECIDED 0
#define SUCCEEDED 1
#define FAILED 2

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
    std::atomic<long int> tmp1{7};
    std::atomic<long int> tmp2{3};
    std::atomic<long int> *a1;
    const long int o1{7};
    std::atomic<long int> *a2;
    const long int o2{3};
    const long int n2{4};
    
    //this is ugly as fuck, but for some reason *a1=7  is just not allowed
    RDCSSDescriptor(long int _a1, long int _a2, long int _o1, long int _o2, long int _n2) : o1(_o1) , o2(_o2), n2(_n2), tmp1(_a1), tmp2(_a2), a1(&tmp1), a2(&tmp2){    }
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

void* SetTag(void *p1) {
    int tag=2;
    const uintptr_t MASK = ~0x03ULL;
    long int *p2=(long int *)(((uintptr_t)p1 & MASK) | tag);
    //std::cout << "set " << p1 << " " << p2 << std::endl;
    return p2;
}

Zeugs GetDescriptor(void *p2) {
    const uintptr_t MASK = ~0x03ULL;
    intptr_t p3 = (uintptr_t)p2 & MASK;
    return *(Zeugs*)p3;
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
    std::cout << "doing CAS on value " << a << " with expected value " << o << " and new value " << n << std::endl;
    a.compare_exchange_weak(o, n);
    std::cout << "CAS resulted in " << a << std::endl;
    return old;
}

void Complete(RDCSSDescriptor *d) {
	long int v = *d->a1;
	std::cout << "v set as " << v << std::endl;
	if(v==d->o1) {
	std::cout << "attempting second CAS1 with a2 " << *d->a2 << " expected value " << (long int)SetTag(d) << " new value " << d->n2 << std::endl;
	CAS1(*d->a2,(long int)SetTag(d),d->n2);
	std::cout << "second CAS1 succeeded with a2 now " << *d->a2 << std::endl;
	}
	else {
	CAS1(*d->a2,(long int)d,d->o2);
	std::cout << "second CAS1 did not succeed, undoing that CAS1" << std::endl;
	}
}

long int RDCSS (RDCSSDescriptor *d) {
    long int r;
    do {
    	std::cout << "a2 before CAS1 " << *d->a2 << std::endl;
        r=CAS1(*d->a2, d->o2, (long int)SetTag(d));
        std::cout << "a2 after CAS1 " << *d->a2 << std::endl;
        std::cout << "first CAS happened, value " << r << " returned, we now have value " << *d->a2 << " at the data location" << std::endl;
        if(IsDescriptor((void*)r)==2 && r!=2) {
        std::cout << "we found a descriptor " << r << " and will complete it now" << std::endl;
        Complete((RDCSSDescriptor*)r);
        }
        std::cout << "r " << r << std::endl;
    } while(IsDescriptor((void*)r)==2 && r!=2);
    if(r==d->o2) {
    std::cout << "CAS1 was successful and we will now complete the operation" << std::endl;
    Complete(d);
    }
    std::cout << "finished RDCSS and have values a1 " << *d->a1 << " a2 " << *d->a2 << std::endl;
    return r;
}

bool CASN(CASNDescriptor *cd){
	if(cd->status == UNDECIDED){
		int status = SUCCEEDED;
		std::cout << "started and set status to succeeded " << status << std::endl;
		for(int i=0; (i<cd->count) && (status == SUCCEEDED); i++){
			auto entry = cd->entry[i];
			std::cout << "chose first entry" << std::endl;
			RDCSSDescriptor temp = RDCSSDescriptor(cd->status, *entry.addr, UNDECIDED, entry.oldVal, (long int)SetTag(cd));
			long int val = RDCSS(&temp);
			std::cout << "value returned from RDCSS on first entry " << val << std::endl;
			if(IsDescriptor((void*)val)==2){
				if(cd == GetDescriptorCASN((void*)val)){
					std::cout << "found a descriptor and doing CASN on that one now" << std::endl;
					CASN(GetDescriptorCASN((void*)val));
					//goto
				}
			}
			else if(val != entry.oldVal){
				status = FAILED;
				std::cout << "failed to assign values" << std::endl;
			}
			std::cout << "finished entry " << i << " of many, looping back" << std::endl << std::endl;
		}
		std::cout << "finished loop through entries, now assigning success status" << std::endl;
		CAS1((cd->status), UNDECIDED, status);
	}
	bool succeeded = (cd->status == SUCCEEDED);
	for(int i=0; i<cd->count; i++){
		CAS1(*(cd->entry[i].addr), (long int)SetTag(cd), succeeded?(cd->entry[i].newVal) : (cd->entry[i].oldVal));
	}
	return succeeded;
}

long int RDCSSRead(void *addr){
	void *r;
	do {
		r = addr;
		if(IsDescriptor(r)) Complete((RDCSSDescriptor*)r);
	} while(IsDescriptor(r));
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

    std::atomic<long int> ato{3};
    std::atomic<long int> ato2{4};
    entries entry = {&ato, 3, 5};
    entries entry2 = {&ato, 4, 5};
    std::cout << "before execution we have ato1 " << ato << " ato2 " << ato2 << std::endl;
    std::vector<entries> ent{entry};
    CASNDescriptor casndesc(ent);
    std::cout << "starting CASN" << std::endl;
    bool ran = CASN(&casndesc);
    std::cout << "CASN resulted in " << ran << std::endl;
    
    std::atomic<long int> *a1,*a2;
    a1 = entry.addr;
    a2 = entry2.addr;
    
    std::cout << "post execution we have ato1 " << ato << " ato2 " << ato2 << std::endl;
    std::cout << "post execution we have a1 " << *a1 << " a2 " << *a2 << std::endl;
	std::cout << "finished execution" << std::endl;
	
    //RDCSSDescriptor desc(7,4,7,4,1);
    //long int temp;
    //temp = RDCSS(&desc);
    
    return 0;
}
