#include "header.h"

Boot_Record::Boot_Record(uint64_t cluster_count, uint16_t cluster_size)
{ 
    this->cluster_size = cluster_size;
    this->cluster_count = cluster_count;
	this->disk_size = cluster_size * cluster_count;

	this->mft_start_cluster = 2;
	size_t mft_end_cluster = (size_t)(0.1 * cluster_count) + this->mft_start_cluster;
	
	this->bitmap_start_cluster = mft_end_cluster + 1;
	size_t bitmap_end_cluster = (((size_t)cluster_count * sizeof(bool)) / cluster_size) + this->bitmap_start_cluster;

	this->data_start_cluster = bitmap_end_cluster + 1;

    this->mft_max_fragment_count = MFT_FRAGMENTS_COUNT;
}
