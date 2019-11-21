#include "header.h"

Mft_Item::Mft_Item(int uid, bool is_directory, std::string name, long item_size, int parent_ID, bool is_symlink, int linked_UID){
    
    this->uid = uid;
    this->is_directory = is_directory;
    strcpy_s(this->item_name, 12, name.c_str());
    this->item_order = 1;
    this->item_order_total = 1;
    this->item_size = item_size;
    this->parent_ID = parent_ID;
    this->is_symlink = is_symlink;
    this->linked_UID = linked_UID;
    for (int i = 0; i < MFT_FRAGMENTS_COUNT; i++) {
        this->fragments[i] = new Mft_Fragment(0, 0, 0);
    }
    
    
}
