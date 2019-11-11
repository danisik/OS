//
//  mft_item.h
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Mft_item_h
#define Mft_item_h

#include "Mft_fragment.h"
#include <stdio.h>
#include <vector>
#include "Constants.h"

class Mft_item
{
public:
    int32_t uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
    int32_t parent_id;
    bool isDirectory;                                   //soubor, nebo adresar
    int8_t item_order;                                  //poradi v MFT pri vice souborech, jinak 1
    int8_t item_order_total;                            //celkovy pocet polozek v MFT
    char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
    int32_t item_size;                                  //velikost souboru v bytech
    std::vector<Mft_fragment*> fragments;               //fragmenty souboru
    int cluster_count;
    int32_t fragments_count;
    
    Mft_item(int32_t uid, int32_t parent_id, bool isDirectory, int8_t item_order, int8_t item_order_total, char *name, int32_t size);
    Mft_item(){};
    void set_up_fragmets(int32_t size);
    void create_fragments(int count);
    void add_fragment(Mft_fragment* fragment);
};
#endif /* mft_item_h */
