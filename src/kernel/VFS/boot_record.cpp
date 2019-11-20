#include "Header.h"

BootRecord::BootRecord(long diskSize){
    strcpy_s(signature, 8, "Danisik");
    strcpy_s(volume_descriptor, 16, "Jsem deskriptor");
    
    this->diskSize = diskSize;
    this->clusterSize = CLUSTER_SIZE;
    this->clusterCount = diskSize/clusterSize;
    this->mftStartAddress = sizeof(BootRecord);
    this->bitmapStartAddress = (long)(mftStartAddress + sizeof(MFT) + 0.1 * diskSize);
    this->dataStartAddress = bitmapStartAddress + sizeof(bool)*clusterCount;
    this->mftMaxFragmentCount = MFT_FRAGMENTS_COUNT;
}
