#ifndef MFT_hpp
#define MFT_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include "mft_item.hpp"

using namespace std;


class MFT{

public:
    MFT();
    bool Is_In_MFT(string);
    vector<Mft_Item*> mft_items;
    int32_t size;
    
};

#endif /* MFT_hpp */
