//
//  Mft_item.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 22/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//


#include "Mft_item.h"

Mft_item::Mft_item(int32_t uid, int32_t parent_id, bool isDirectory, int8_t item_order, int8_t item_order_total, char *name, int32_t size)
{
    this->uid = uid;
    this->parent_id = parent_id;
    this->isDirectory = isDirectory;
    this->item_order = item_order;
    this->item_order_total = item_order_total;
    strcpy(this->item_name, name);
    this->item_size = size;
    this->fragments_count = 0;
}

void Mft_item::add_fragment(Mft_fragment *fragment)
{
    this->fragments_count = this->fragments_count + 1 ;
    this->fragments.push_back(fragment);
}
