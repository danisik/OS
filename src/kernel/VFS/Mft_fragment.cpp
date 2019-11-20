#include "Header.h"

Mft_Fragment::Mft_Fragment(int bitmap_start_ID, int32_t start_address, int32_t count){
    this->bitmap_start_ID = bitmap_start_ID;
    this->fragment_start_address = start_address;
    this->fragment_count = count;
}
