#include<vector>
#include <iostream>
#include <string>
#include <atomic>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <climits>
using word_t = long int;
word_t SUCCEEDED= 2;
word_t UNDECIDED= 1;
word_t FAILED= 0;

struct RDCSS_desc {
    std::atomic<word_t> *addr1;
    std::atomic<word_t> *addr2;
    word_t exp1;
    word_t exp2;
    word_t new2;
    //char padding[24];
    RDCSS_desc(std::atomic<word_t> *addr1, std::atomic<word_t> *addr2, word_t exp1, word_t exp2, word_t new2) {
        this->addr1=addr1;
        this->addr2=addr2;
        this->exp1=exp1;
        this->exp2=exp2;
        this->new2=new2;
    }

} ;

struct CASN_entry {
    std::atomic<word_t> *addr;
    word_t exp1;
    word_t new1;
    CASN_entry(std::atomic<word_t> *addr, word_t exp1, word_t new1) {
        this->addr=addr;
        this->exp1=exp1;
        this->new1=new1;
    }
};

struct CASN_desc {
    std::atomic<word_t> status;
    word_t n;
    std::vector<CASN_entry> entry;
    //char padding[24];
    CASN_desc(word_t stat, std::vector<CASN_entry> entry0) {
        this->status=stat;
        this->n=entry0.size();
        this->entry=entry0;
    }
};// __attribute__ ((alligned(64)));

word_t* pack(RDCSS_desc *p1) {
    int tag=1;
    const uintptr_t MASK = ~0x03ULL;
    word_t *p2=(word_t*)(((uintptr_t)p1 & MASK) | tag);
    //std::cout << "set " << p1 << " " << p2 << std::endl;
    return p2;
}

word_t* packCASN(CASN_desc *p1) {
    int tag=2;
    const uintptr_t MASK = ~0x03ULL;
    word_t *p2=(word_t *)(((uintptr_t)p1 & MASK) | tag);
    //std::cout << "packCASN" << p1 << " " << p2 << std::endl;
    return p2;
}

RDCSS_desc *unpack(word_t *p1) {
    //std::cout << "unpackRDCSS";
    const uintptr_t MASK = ~0x03ULL;
    intptr_t p2 = (uintptr_t)p1 & MASK;
    //std::cout << "unpack" << (uintptr_t)p1 << " " << (uintptr_t)p2 << std::endl;
    return (RDCSS_desc*)p2;
}

CASN_desc *unpackCASN(word_t *p1) {
    const uintptr_t MASK = ~0x03ULL;
    intptr_t p2 = (uintptr_t)p1 & MASK;
    //std::cout << "unpackCASN" << (uintptr_t *)p1 << " " << (uintptr_t *)p2 << std::endl;
    return (CASN_desc*)p2;
}

word_t CAS1(std::atomic<word_t> &a, word_t expected, word_t n) {
    word_t old=(word_t) a; 
    //if (old==o) *a=n;   
    a.compare_exchange_weak(expected, n);

    //std::cout << "CAS1 old:" << old << " exp:" << expected << " new:" << n <<  std::endl;

    return old;
}

word_t CAS1_(std::atomic<word_t> &a, word_t expected, word_t n) {
    word_t old=(word_t) a; 
    //if (old==o) *a=n;   
    a.compare_exchange_weak(expected, n);

    std::cout << std::hex << "CAS1 old: " << old << " exp:" << expected << " new:" << n <<  std::endl;

    /*if(a.compare_exchange_weak(expected, n)) {
        std::cout << "CAS1 succ " << std::endl;
    } else {
        std::cout << "CAS1 fail " << std::endl;
    }*/
    return old;
}

void Complete(RDCSS_desc *d) {
    
    if (*d->addr1 == d->exp1) {
        //std::cout << "CompleteA " << d->new2 << std::endl;
        CAS1(*d->addr2, (word_t) pack(d), d->new2);
    } else {
        //std::cout << "CompleteB";
        CAS1(*d->addr2, (word_t) pack(d), d->exp2);
    }
}

bool isRDCSS(word_t *p1) {
    int tag = (uintptr_t)p1 & 0x03;
    //std::cout << p1;
    return (tag==1 && (word_t)p1 > INT_MAX);
}

bool isCASN(word_t *p1) {
    int tag = (uintptr_t)p1 & 0x03;
    //std::cout << (word_t)p1 << " " << tag;
    return (tag==2 && (word_t)p1 > INT_MAX);
}


word_t RDCSSRead(std::atomic<word_t> *addr) {
    word_t v;
    while(true) {
        v=(word_t) *addr;
        if (isRDCSS((word_t*) v)) Complete(unpack((word_t*)v));
        else break;
    }
    return v;
}


word_t RDCSS(std::atomic<word_t> *addr1, std::atomic<word_t> *addr2, word_t exp1, word_t exp2, word_t new2) {
    RDCSS_desc *d=new RDCSS_desc(addr1, addr2, exp1, exp2, new2);
    word_t val2;
    while(true) {
        //std::cout << *d->addr2 << " " << d->exp2 << " " << std::hex << (word_t) pack(d) << std::endl;
        val2=CAS1(*d->addr2, d->exp2, (word_t) pack(d));
        
        if (isRDCSS((word_t*) val2)) {
            //std::cout << std::endl << "found RDCSS Descriptor " << std::hex << val2 << std::endl;
            Complete(unpack((word_t*) val2));
        }
        else break;
    }
    if (val2==d->exp2) {
        Complete(d);
    }
    return val2;    
} 
  

bool CASN_(CASN_desc *d) {
    if (d->status == UNDECIDED) {
        
        word_t stat=SUCCEEDED;
        for (int i=0; (i< d->n) && (stat==SUCCEEDED); i++) {
            
            word_t val2=RDCSS(&d->status, d->entry[i].addr, UNDECIDED, d->entry[i].exp1, (word_t) packCASN(d));
            if (val2 != d->entry[i].exp1) {
                //std::cout << std::endl << " ! ";
                if (isCASN((word_t*)val2)) {
                    //std::cout << std::endl << "found CASN Descriptor" << std::endl;
                    if(unpackCASN((word_t*)val2)!=d) {
                        CASN_(unpackCASN((word_t*)val2));
                        --i;
                        continue;
                    } 
                }
                else {
                	stat=FAILED;
                    break;
                }
            }
        }
        CAS1(d->status, UNDECIDED, stat);

    }
    bool succ=(d->status==SUCCEEDED);
    word_t val;
    for (int i=0; i<d->n; i++) {
        if (succ) {
            val=d->entry[i].new1;
        } else {
            val=d->entry[i].exp1;
        }
        //std::cout << std::endl << "CASN pack: " << *d->entry[i].addr << " " <<  packCASN(d);
        CAS1(*d->entry[i].addr, (word_t) packCASN(d), val);
    }
    return succ;
}

bool CASN(std::vector<CASN_entry> entries) {
    sort(entries.begin(), entries.end(), [](const CASN_entry &left, const CASN_entry &right)
        { return left.addr < right.addr; });

    CASN_desc *d = new CASN_desc(UNDECIDED, entries);
    
    return CASN_(d);

}    

word_t CASNRead(std::atomic<word_t> *addr) {
    word_t v;
    do {
        v=RDCSSRead(addr);
        if (isCASN((word_t*) v)) CASN_(unpackCASN((word_t*) v));
    } while(isCASN((word_t *) v));
    return v;
}

/**/
