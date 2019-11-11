//
//  mft_fragment.h
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Mft_fragment_h
#define Mft_fragment_h

#include <iostream>
#include <string.h>

class Mft_fragment
{
public:
    int32_t fragment_start_address;     //start adresa
    int32_t fragment_count;             //pocet clusteru ve fragmentu
    int32_t fragment_start_index;
    
    Mft_fragment(int32_t fragment_start_address, int32_t fragment_count, int32_t fragment_start_index);
    Mft_fragment(){};
};

#endif /* mft_fragment_h */
