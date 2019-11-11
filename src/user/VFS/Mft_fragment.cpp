//
//  Mft_fragment.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 24/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include <stdio.h>
#include "Mft_fragment.h"

Mft_fragment::Mft_fragment(int32_t fragment_start_address, int32_t fragment_count, int32_t fragment_start_index)
{
    this->fragment_start_address = fragment_start_address;
    this->fragment_count = fragment_count;
    this->fragment_start_index = fragment_start_index;
}
