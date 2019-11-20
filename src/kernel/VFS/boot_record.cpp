#include "header.h"

Boot_Record::Boot_Record(uint64_t cluster_count, uint16_t cluster_size){
    strcpy_s(signature, 8, "Danisik");
    strcpy_s(volume_descriptor, 16, "Jsem deskriptor");
    
    this->cluster_size = cluster_size;
    this->cluster_count = cluster_count;
	this->disk_size = cluster_size * cluster_count;
    this->mft_start_address = sizeof(Boot_Record);
    this->bitmap_start_address = (long)(mft_start_address + sizeof(MFT) + 0.1 * disk_size);
    this->data_start_address = bitmap_start_address + sizeof(bool)*(long)(cluster_count);
    this->mft_max_fragment_count = MFT_FRAGMENTS_COUNT;
}
