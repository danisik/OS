#ifndef mft_fragment_hpp
#define mft_fragment_hpp

#include <stdio.h>

class Mft_Fragment {
public:
    Mft_Fragment(int, int32_t, int32_t);
    int32_t fragment_start_address;     //start adresa
    int32_t fragment_count;             //pocet clusteru ve fragmentu
    int bitmap_start_ID;
};

#endif /* mft_fragment_hpp */
