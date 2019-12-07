#include "header.h"

Mft_Item::Mft_Item(size_t uid, kiv_os::NFile_Attributes is_directory, std::string name, size_t item_size, size_t parent_ID){
    
    this->uid = uid;
    this->is_directory = is_directory;
    strcpy_s(this->item_name, 12, name.c_str());
    this->item_order = 1;
    this->item_order_total = 1;
    this->item_size = item_size;
    this->parent_ID = parent_ID;
	
	for (int i = 0; i < MFT_FRAGMENTS_COUNT; i++) {
		this->bitmap_start_ID[i] = 0;
		this->fragment_start_cluster[i] = 0;
		this->fragment_cluster_count[i] = 0;
	}
}
