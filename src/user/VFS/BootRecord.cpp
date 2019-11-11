//
//  BootRecord.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include "BootRecord.hpp"

BootRecord::BootRecord(int32_t disk_size, int32_t cluster_size)
{
    strncpy(this->signature, "cagy", sizeof(this->signature) - 1);
    strncpy(this->volume_descriptor, "pseudoNTFS", sizeof(this->volume_descriptor) - 1);
    
    this->disk_size = disk_size;
    this->cluster_size = cluster_size;
    this->cluster_count = disk_size / cluster_size;
    this->mft_start_address = sizeof(BootRecord);
    this->bitmap_start_address = sizeof(BootRecord) + (disk_size * MFT_PERCENTAGE_USAGE);
    
    this->data_start_address =sizeof(BootRecord) + this->bitmap_start_address + sizeof(Bitmap);
    
    this->print_bootrecord_details();

}

void BootRecord::print_bootrecord_details()
{
    std::cout << "BOOTRECORD" << std::endl;
    std::cout << "Disk size: " << std::to_string(this->disk_size) << " B" << std::endl;
    std::cout << "Cluster size: " << std::to_string(this->cluster_size) << " B" << std::endl;
    std::cout << "Cluster count: " << std::to_string(this->cluster_count) << std::endl;
    std::cout << "MFT start address: " << std::to_string(this->mft_start_address) << std::endl;
    std::cout << "Bitmap start address: " << std::to_string(this->bitmap_start_address) << std::endl;
    std::cout << "Data start address: " << std::to_string(this->data_start_address) << std::endl;
}

void BootRecord::write(FILE *file)
{
    fseek(file, 0, SEEK_SET);
    fwrite(this, sizeof(BootRecord), 1, file);
    fflush(file);
}
