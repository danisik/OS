#include "header.h"

VFS::VFS(FILE* file, uint64_t cluster_count, uint16_t cluster_size){
    this->file = file;
    this->boot_record = new Boot_Record(cluster_count, cluster_size);
    this->mft = new MFT();
    this->bitmap = new bool[boot_record->Get_Cluster_Count()];
    for (int i = 0; i<this->boot_record->Get_Cluster_Count(); i++) {
        this->bitmap[i] = false;
    }
}


void VFS::Print_Current_Path(VFS *vfs){
    cout<< ROOT_CHAR;
    for (size_t i = 0; i<vfs->current_path.size(); i++) {
        cout << vfs->current_path[i]->item_name << "/";
    }
}
