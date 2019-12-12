#include "header.h"

Mft_Fragment::Mft_Fragment(int bitmap_start_ID, size_t start_cluster, size_t cluster_count)
{
    this->bitmap_start_ID = bitmap_start_ID;
    this->fragment_start_cluster = start_cluster;
    this->fragment_cluster_count = cluster_count;
}
