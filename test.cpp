#include <iostream>
#include <string>
#include <atomic>
#include <vector>

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
    long int *a1;
    const long int o1;
    long int *a2;
    const long int o2;
    const long int n2;
};

class CASNDescriptor {
    public:
    int status;
    int count; 
    struct entries {
        long int addr;
        long int oldVal;
        long int newVal;
    };
    std::vector<entries> entry;

    void resize(int size){
        count=size;
		entry.resize(size);		
	};

};


int IsDescriptor(void *p1) {
    //void *p1 = &r; // the pointer we want to store the value into
    std::cout << p1 << std::endl;
    int tag = (uintptr_t)p1 & 0x03;
    return tag;
}

void* SetTag(void *p1) {
    int tag=2;
    const uintptr_t MASK = ~0x03ULL;
    long int *p2=(long int *)(((uintptr_t)p1 & MASK) | tag);
    std::cout << "set" << p1 << " " << p2 << std::endl;
    return p2;
}

Zeugs GetDescriptor(void *p2) {
    const uintptr_t MASK = ~0x03ULL;
    intptr_t p3 = (uintptr_t)p2 & MASK;
    return *(Zeugs*)p3;
}

long int CAS1(std::atomic<long int> &a, long int o, long int n) {
    long int old=a;  
    a.compare_exchange_weak(o, n);
    return old;
}

/*long int RDCSS (RDCSSDescriptor *d) {
    long int r;
    do {
        r=1;
    } while(false)
}*/

int main() {
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

    return 0;
}