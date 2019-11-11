//
//  BootRecord.hpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef BootRecord_hpp
#define BootRecord_hpp

#include <stdio.h>
#include <cstdlib>
#include <cstring>

#include "Constants.h"
#include "Bitmap.hpp"
#include "Mft_item.h"

class BootRecord
{
    
public:
    BootRecord(int32_t disk_size, int32_t cluster_size);
    BootRecord(){};
    
    char signature[9];              //login autora FS
    char volume_descriptor[251];    //popis vygenerovaného FS
    int32_t disk_size;              //celkova velikost VFS
    int32_t cluster_size;           //velikost clusteru
    int32_t cluster_count;          //pocet clusteru
    int32_t mft_start_address;      //adresa pocatku mft
    int32_t bitmap_start_address;   //adresa pocatku bitmapy
    int32_t data_start_address;     //adresa pocatku datovych bloku
    int32_t mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
    // stejne jako   MFT_FRAGMENTS_COUNT
    void print_bootrecord_details();
    long mft_items_count;
    
    void write(FILE* file);
    
};
#endif /* BootRecord_hpp */
