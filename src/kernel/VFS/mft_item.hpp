#ifndef mft_item_hpp
#define mft_item_hpp

#include <stdio.h>
#include <string>
#include "Constants.hpp"
#include "mft_fragment.hpp"
#include "mft_item.hpp"
#include "boot_record.hpp"


using namespace std;

const int32_t UID_ITEM_FREE = 0;

class MftItem {

public:
    MftItem(int, bool, string, long, int);
    
    int uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
    int parentID;
    bool isDirectory;                                   //soubor, nebo adresar
    int item_order;                                  //poradi v MFT pri vice souborech, jinak 1
    int item_order_total;                            //celkovy pocet polozek v MFT
    char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
    long item_size;                                  //velikost souboru v bytech
    MftFragment* fragments[MFT_FRAGMENTS_COUNT]; //fragmenty souboru
};


#endif /* mft_item_hpp */
