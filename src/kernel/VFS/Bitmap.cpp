//
//  Bitmap.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include "Bitmap.hpp"

Bitmap::Bitmap(int32_t size)
{
    printf("Bitmap size: %d\n", size);
    this->size = size;
    this->map = new bool[size];
    
    //set all values to false
    this->reset_bitmap();
}

int Bitmap::find_free_cluster(int start_from)
{
    for (int i = start_from ; i < this->size; i++)
    {
        if (!this->map[i])
        {
            return i;
        }
    }
    
    return -1;
}

bool Bitmap::is_cluster_free(int position)
{
    return !this->map[position];
}

void Bitmap::set_as_used(int index)
{
    this->map[index] = true;
}

void Bitmap::set_as_free(int index)
{
    this->map[index] = false;
}

void Bitmap::reset_bitmap()
{
    for (int i = 0 ; i < this->size; i++)
    {
        this->map[i] = false; // false -> empty cluster
    }
}

int Bitmap::free_cluster_count()
{
    int count = 0;
    
    for (int i = 0 ; i < this->size; i++)
    {
        if (this->map[i] == false)
        {
            count++;
        }
    }
    
    return count;
}

void Bitmap::write(FILE *file, int32_t address)
{
    fseek(file, address, SEEK_SET);
    fwrite(this->map, sizeof(bool), this->size, file);
    fflush(file);
}

void Bitmap::print()
{
    for (int i = 0 ; i < this->size; i++)
    {
        std::cout << i << " " << this->map[i] << std::endl;
    }
}
