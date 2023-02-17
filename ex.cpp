#include <iostream>
#include <vector>


template<typename T>
void dumpvecbytes(const std::vector<T>& vec) {
    const unsigned char *pdata = reinterpret_cast< const unsigned char *>(vec.data());
    int nbytes = vec.size()*sizeof(T);
    printf("%d bytes\n", nbytes);
    for (int i = 0; i < nbytes; ++i) {
        printf("%02x ", pdata[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
    std::cout << std::endl;
}

template<typename T>
void dumpvec(const std::vector<T>& vec) {
    printf("Vector capacity=%lu size=%lu content\n", vec.capacity(), vec.size());
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    //std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}

static int t1() {
    // create a vector of integers
    std::vector<int> myVector = {1, 2, 3, 4, 5};
    //myVector.resize(100);
    myVector.reserve(100);
    dumpvec( myVector);
    myVector[2]=19;
    dumpvec( myVector);
    auto vit = &myVector[2];
    *vit++ = 38;
    dumpvec( myVector);
    *vit++ = 49;
    dumpvec( myVector);
 
    myVector.push_back(99);
    dumpvec( myVector);
    //myVector.empty();
    myVector.clear();
    dumpvec( myVector);
    myVector.push_back(1234);
    dumpvec( myVector);
    return 0;
}

static void t2() {
    // create a vector of integers
    std::vector<int> vec = {
        1, 2, 3, 4, 5,
        6, 7, 8, 9, 10
    };
    auto dp = &vec[5];
    for (int i=0; i<3;i++) {
        printf("dp[%d]=%u\n", i, dp[i]);
        dp[i]+=100;
    }
    dumpvec(vec);

    dp = &vec[1];
    for (int i=0; i<3;i++) {
        *dp+=10;
        printf("inc %d =%u\n", i, *dp);
        dp++;
    }
    dumpvec(vec);

}
static void t3() {
    // create a vector of integers
    std::vector<int> vec = {
        1, 2, 3, 4, 5,
        6, 7, 8, 9, 10
    };
    //const auto dp = &vec[5];
    auto it = vec.begin();
    it+=9;
    printf("it > vec.begin :%s\n", it>vec.begin() ? "true":"false");
    printf("it < vec.end :%s\n", it<vec.end() ? "true":"false");
    /*
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }*/
    //dumpvec(vec);
}

static void t4() {
    std::vector<int> vec = {
        1, 2, 3, 4, 5,
        6, 7, 8, 9, 10
    };
    for ( const int *dp = &vec[5]; dp < vec.data()+vec.size(); dp++) {
        printf("dp ++=%u\n", *dp);
    }
}

static void OneBppEncode( const float *srcpix, char *encpix) {
    for (int i=0;i<3;i++) 
        encpix[i] = srcpix[i]>0.5 ? '1':'0';
}

static void t5() {
    std::vector<float> srcpix={ 0.1, 0.6, 0.5,0.8,0.9};
    std::vector<char> dstpix;

    dstpix.resize(10);
    OneBppEncode( srcpix.data(), dstpix.data());
    OneBppEncode( srcpix.data(), dstpix.data()+5);
    for (auto it = dstpix.begin(); it != dstpix.end(); ++it) {
        std::cout <<  *it << ", ";
    }
    std::cout << std::endl;
}

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;     /* fixed BM */
    uint32_t bfSize;     /* file size */
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;  /* offset of pixels aka total length of headers*/ 
} BITMAPFILEHEADER;
#pragma pack(pop)


template<typename T>
void append_struct( std::vector<char> &vecd, T &st) {
    const unsigned char *pdata = reinterpret_cast< const unsigned char *>(&st);
    const char* dbegin = reinterpret_cast<const char*>(&st);
    const char* dend = dbegin + sizeof(T);
    vecd.insert( vecd.end(), dbegin, dend); 
}

static void t6() {
    std::vector<char> data;
    BITMAPFILEHEADER bmh;
    append_struct( data, bmh);
    dumpvecbytes(data);
}

// aliasing
static void t7() {
    std::vector<char> vec(48);
    BITMAPFILEHEADER *pbmh = (BITMAPFILEHEADER *)(vec.data()+16);
    pbmh->bfType=0x04;     /* fixed BM */
    pbmh->bfSize=0x12345678;
    dumpvecbytes(vec);
}

int main() {
    t7();
    return 0;
} 
