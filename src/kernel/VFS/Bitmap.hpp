//
//  Bitmap.hpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Bitmap_hpp
#define Bitmap_hpp

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

class Bitmap
{
public:
    Bitmap(int32_t size);
    int32_t size;
    bool* map;
    
    int find_free_cluster(int start_from);
    bool is_cluster_free(int position);
    void set_as_used(int index);
    void set_as_free(int index);
    void reset_bitmap();
    int free_cluster_count();
    void write(FILE *file, int32_t address);
    void print();
};


#endif /* Bitmap_hpp */
