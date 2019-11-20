#include "Header.h"

Boot_Record::Boot_Record(long disk_size){
    strcpy_s(signature, 8, "Danisik");
    strcpy_s(volume_descriptor, 16, "Jsem deskriptor");
    
    this->disk_size = disk_size;
    this->cluster_size = CLUSTER_SIZE;
    this->cluster_count = disk_size /cluster_size;
    this->mft_start_address = sizeof(Boot_Record);
    this->bitmap_start_address = (long)(mft_start_address + sizeof(MFT) + 0.1 * disk_size);
    this->data_start_address = bitmap_start_address + sizeof(bool)*cluster_count;
    this->mft_max_fragment_count = MFT_FRAGMENTS_COUNT;
}
