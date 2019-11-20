#include "Header.h"

VFS::VFS(FILE* file, long diskSize){
    this->file = file;
    this->bootRecord = new BootRecord(diskSize);
    this->mft = new MFT();
    this->bitmap = new bool[bootRecord->getClusterCount()];
    for (int i = 0; i<this->bootRecord->getClusterCount(); i++) {
        this->bitmap[i] = false;
    }
}


void VFS::printCurrentPath(VFS *vfs){
    cout<< ROOT_CHAR;
    for (size_t i = 0; i<vfs->currentPath.size(); i++) {
        cout << vfs->currentPath[i]->item_name << "/";
    }
}
