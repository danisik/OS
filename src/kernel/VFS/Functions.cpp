#include "header.h"



Exist_Item* Functions::Check_Path(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path){
    Exist_Item* item = new Exist_Item;
    item->uid = current_path[current_path.size()-1]->uid;
    item->parent_ID = current_path[current_path.size()-1]->parent_ID;
    item->exists = false;
    item->path_exists = false;
    item->is_directory = true;
    size_t i;
	std::string tok;
    while((i = path.find(FOLDER_SPLIT)) != std::string::npos){
        item->path_exists = false;
        tok = path.substr(0, i);
        path = path.substr(i+1);
        for(size_t j = 0; j < vfs->mft->mft_items.size(); j++){
            if((vfs->mft->mft_items[j]->parent_ID == item->uid)
               &&(strcmp(vfs->mft->mft_items[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft->mft_items[j]->uid;
                item->parent_ID = vfs->mft->mft_items[j]->parent_ID;
                item->path_exists = true;
                if(vfs->mft->mft_items[j]->is_directory){
                    item->path_exists = true;
                    item->is_directory = true;
                }
                else {
                    item->path_exists = false;
                    item->is_directory = false;
                    return item;
                }
                break;
            }
        }
        if(!item->path_exists){
            return item;
        }
        
    }
    tok = path.substr(0,i);
    item->path_exists = true;
    for(size_t j = 0; j < vfs->mft->mft_items.size(); j++){
        if((vfs->mft->mft_items[j]->parent_ID == item->uid)
           &&(strcmp(vfs->mft->mft_items[j]->item_name, tok.c_str())==0)){
            item->uid = vfs->mft->mft_items[j]->uid;
            item->parent_ID = vfs->mft->mft_items[j]->parent_ID;
            item->exists = true;
            if(vfs->mft->mft_items[j]->is_directory) item->is_directory = true;
            else item->is_directory = false;
            return item;
        }
    }
    item->exists=false;
    return item;
}

bool Functions::Is_Directory_Empty(VFS* vfs, Exist_Item * item){
    for(size_t i = 0; i < vfs->mft->mft_items.size(); i++){
        if(vfs->mft->mft_items[i]->parent_ID == item->uid){
            return false;
        }
    }
    return true;
}
void Functions::Move_To_Path(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path){
    Exist_Item* item = new Exist_Item;
    item->uid = current_path[current_path.size()-1]->uid;
    item->parent_ID = current_path[current_path.size()-1]->parent_ID;
    size_t i;
	std::string tok;
    while((i = path.find(FOLDER_SPLIT)) != std::string::npos){
        tok = path.substr(0, i);
        path = path.substr(i+1);
        for(size_t j = 0; j < vfs->mft->mft_items.size(); j++){
            if((vfs->mft->mft_items[j]->parent_ID == item->uid)
               &&(strcmp(vfs->mft->mft_items[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft->mft_items[j]->uid;
                item->parent_ID = vfs->mft->mft_items[j]->parent_ID;
				current_path.push_back(vfs->mft->mft_items[j]);
                break;
            }
        }
        
    }
    tok = path.substr(0,i);
    for(size_t j = 0; j < vfs->mft->mft_items.size(); j++){
        if((vfs->mft->mft_items[j]->parent_ID == item->uid)
           &&(strcmp(vfs->mft->mft_items[j]->item_name, tok.c_str())==0)){
            item->uid = vfs->mft->mft_items[j]->uid;
            item->parent_ID = vfs->mft->mft_items[j]->parent_ID;
            item->exists = true;
			current_path.push_back(vfs->mft->mft_items[j]);
            return;
        }
    }
}

bool Functions::Is_Bitmap_Writable(VFS* vfs, long size){
    long neededBitCount = size/vfs->boot_record->cluster_size;
    if(size % vfs->boot_record->cluster_size != 0) neededBitCount++;
    long bitCount = 0;
    for(int i = 0; i < vfs->boot_record->cluster_count; i++){
        if(!vfs->bitmap[i]) bitCount++;
        if(bitCount==neededBitCount) return true;
    }
    return false;
}

void Functions::Write_To_Data_Block(VFS * vfs, Mft_Item * mftItem){
    vfs->mft->mft_items.push_back(mftItem);
    long size = mftItem->item_size;
    long neededBitCount = size/vfs->boot_record->cluster_size;
    if(size % vfs->boot_record->cluster_size != 0) neededBitCount++;
    long writtenBitCount = 0;
    int fragmentID = 0;
    
    for (int i = 0; i < vfs->boot_record->cluster_count; i++) {
        if(writtenBitCount == neededBitCount) break;
        
        if(!vfs->bitmap[i]){
            if(mftItem->fragments[fragmentID]->fragment_count<1){
                mftItem->fragments[fragmentID]->bitmap_start_ID = i;
                mftItem->fragments[fragmentID]->fragment_start_address = vfs->boot_record->data_start_cluster + i*vfs->boot_record->cluster_size;
                mftItem->fragments[fragmentID]->fragment_count++;
                vfs->bitmap[i] = true;
            }
            else{
                mftItem->fragments[fragmentID]->fragment_count++;
                vfs->bitmap[i] = true;
            }
            writtenBitCount++;
        }
        else{
            if(mftItem->fragments[fragmentID]->fragment_count > 0){
                fragmentID++;
            }
        }
    }
}
void Functions::Write_To_Clusters(VFS * vfs, Mft_Item* mftItem, FILE* file){
    int fragmentID = 0;
    int filePointer = 0;
    long size = mftItem->item_size;
    char buffer[CLUSTER_SIZE];
    while (mftItem->fragments[fragmentID]->fragment_start_address != 0) {
        for (int j = 0; j<mftItem->fragments[fragmentID]->fragment_count; j++) {
            
            fseek(file, filePointer*vfs->boot_record->cluster_size, SEEK_SET);
            fread(buffer, vfs->boot_record->cluster_size, 1, file);
            fseek(vfs->file, mftItem->fragments[fragmentID]->fragment_start_address + j*vfs->boot_record->cluster_size, SEEK_SET);
            if(size>=vfs->boot_record->cluster_size){
                fwrite(buffer, vfs->boot_record->cluster_size, 1, vfs->file);
                size = size - vfs->boot_record->cluster_size;
                filePointer++;
            }
            else{
                fwrite(buffer, size, 1, vfs->file);
                return;
            }
        }
        fragmentID++;
    }
}

void Functions::Copy_To_Clusters(VFS* vfs, Mft_Item* srcFile, Mft_Item* destFile){
    int fragmentID = 0;
    int filePointer;
    long size = srcFile->item_size;
    int bufferPointer = 0;
	std::vector<std::string> bufferVector;
    char buffer[CLUSTER_SIZE];
    while (srcFile->fragments[fragmentID]->fragment_start_address != 0) {
        filePointer = srcFile->fragments[fragmentID]->fragment_start_address;
        for (int j = 0; j<srcFile->fragments[fragmentID]->fragment_count; j++) {
            
            fseek(vfs->file, filePointer+j*vfs->boot_record->cluster_size, SEEK_SET);
            memset(buffer, 0, vfs->boot_record->cluster_size);
            fread(buffer, vfs->boot_record->cluster_size, 1, vfs->file);
            bufferVector.push_back(buffer);
        }
        fragmentID++;
    }
    fragmentID = 0;
    while (destFile->fragments[fragmentID]->fragment_start_address != 0) {
        filePointer = destFile->fragments[fragmentID]->fragment_start_address;
        for (int j = 0; j<destFile->fragments[fragmentID]->fragment_count; j++) {
            memset(buffer, 0, vfs->boot_record->cluster_size);
            fseek(vfs->file, filePointer + j*vfs->boot_record->cluster_size, SEEK_SET);
            if(size>=vfs->boot_record->cluster_size){
                strcpy_s(buffer, sizeof(char*), bufferVector[bufferPointer].data());
                fwrite(buffer, vfs->boot_record->cluster_size, 1, vfs->file);
                bufferPointer++;
                size = size - vfs->boot_record->cluster_size;
            }
            else{
                strcpy_s(buffer, sizeof(char), bufferVector[bufferPointer].data());
                fwrite(buffer, size, 1, vfs->file);
                return;
            }
        }
        fragmentID++;
    }
}
void Functions::Print_Bitmap(VFS * vfs){
	for (int i = 0; i<vfs->boot_record->cluster_count; i++) {
		if(vfs->bitmap[i]){
			std::cout << "1";
		}
		else{
			std::cout <<"0";
		}
	}
	std::cout << std::endl << std::endl;
}

void Functions::Remove_From_Data_Block(VFS* vfs, Mft_Item* mftItem){
    for (int i = 0; i < MFT_FRAGMENTS_COUNT; i++) {
        for (int j = 0; j<mftItem->fragments[i]->fragment_count; j++) {
            vfs->bitmap[mftItem->fragments[i]->bitmap_start_ID+j] = false;
        }
    }
}

void Functions::Save_Vfs_To_File(VFS* vfs){
    //zapis bootrecordu
    fseek(vfs->file, 0, SEEK_SET);
    fwrite(vfs->boot_record, sizeof(Boot_Record), 1, vfs->file);
    fflush(vfs->file);
    //zapis mft
    fseek(vfs->file, vfs->boot_record->mft_start_cluster, SEEK_SET);
    fwrite(vfs->mft, sizeof(MFT), 1, vfs->file);
    fflush(vfs->file);
    //zapis mftItems
    for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
        fseek(vfs->file, (long)(vfs->boot_record->mft_start_cluster+sizeof(MFT)+i*sizeof(Mft_Item)+ i*vfs->boot_record->mft_max_fragment_count*sizeof(Mft_Fragment)), SEEK_SET);
        fwrite(vfs->mft->mft_items[i], sizeof(Mft_Item), 1, vfs->file);
        fflush(vfs->file);
        for (int j = 0; j<vfs->boot_record->mft_max_fragment_count; j++) {
            fseek(vfs->file, (long)(vfs->boot_record->mft_start_cluster+sizeof(MFT)+i*sizeof(Mft_Item)+i*vfs->boot_record->mft_max_fragment_count*sizeof(Mft_Fragment) +sizeof(Mft_Item)+j*sizeof(Mft_Fragment)), SEEK_SET);
            fwrite(vfs->mft->mft_items[i]->fragments[j], sizeof(Mft_Fragment), 1, vfs->file);
            fflush(vfs->file);
        }
    }
    //zapis bitmapy
    fseek(vfs->file, vfs->boot_record->bitmap_start_cluster, SEEK_SET);
	//TODO
    //fwrite(vfs->bitmap, sizeof(bool), vfs->boot_record->cluster_count, vfs->file);
    fflush(vfs->file);
}
void Functions::Print_Clusters(VFS * vfs, Mft_Item * mftItem){
    int fragmentID = 0;
    int filePointer = 0;
    long size = mftItem->item_size;
    char buffer[CLUSTER_SIZE];
    while (mftItem->fragments[fragmentID]->fragment_start_address != 0) {
        for (int j = 0; j<mftItem->fragments[fragmentID]->fragment_count; j++) {
            
            fseek(vfs->file, mftItem->fragments[fragmentID]->fragment_start_address + j*vfs->boot_record->cluster_size, SEEK_SET);
            if(size>=vfs->boot_record->cluster_size){
                fread(buffer, vfs->boot_record->cluster_size, 1, vfs->file);
                for(int i = 0; i<vfs->boot_record->cluster_size;i++){
					std::cout << buffer[i];
                }
                size = size - vfs->boot_record->cluster_size;
                filePointer++;
            }
            else{
                memset(buffer, 0, vfs->boot_record->cluster_size);
                fread(buffer, size, 1, vfs->file);
                for(int i = 0; i<size;i++){
					std::cout << buffer[i];
                }
				std::cout << std::endl;
                return;
            }
        }
        fragmentID++;
    }
}

VFS* Functions::Load_VFS(FILE * file){
    //bootrecord
	// TODO Load_VFS: change 1??
    Boot_Record *bootRecord = new Boot_Record(1, 1);
    fseek(file, 0, SEEK_SET);
    fread(bootRecord, sizeof(Boot_Record), 1, file);
    VFS* vfs = new VFS(bootRecord->cluster_count, bootRecord->cluster_size);
    vfs->boot_record = bootRecord;
    //bitmap
    fseek(vfs->file, vfs->boot_record->bitmap_start_cluster, SEEK_SET);
    //TODO
	//fread(vfs->bitmap, sizeof(bool), vfs->boot_record->Get_Cluster_Count(), vfs->file);
    //MFT
    MFT* mft = new MFT();
    fseek(vfs->file, vfs->boot_record->mft_start_cluster, SEEK_SET);
    fread(mft, sizeof(MFT), 1, vfs->file);
    vfs->mft->size = mft->size;
    vfs->mft->UID_counter = mft->UID_counter;
    //MFTItems
    for (int i = 0; i<=vfs->mft->size; i++) {
        fseek(vfs->file,vfs->boot_record->mft_start_cluster+sizeof(MFT)+i*sizeof(Mft_Item)+ i*vfs->boot_record->mft_max_fragment_count*sizeof(Mft_Fragment), SEEK_SET);
        Mft_Item* item = new Mft_Item(0, false, "", 0, 0, false, 0);
        fread(item, sizeof(Mft_Item), 1, vfs->file);
        for (int j = 0; j<vfs->boot_record->mft_max_fragment_count; j++) {
            fseek(vfs->file, vfs->boot_record->mft_start_cluster+sizeof(MFT)+i*sizeof(Mft_Item)+i*vfs->boot_record->mft_max_fragment_count*sizeof(Mft_Fragment) +sizeof(Mft_Item)+j*sizeof(Mft_Fragment), SEEK_SET);
            Mft_Fragment * fragment = new Mft_Fragment(0, 0, 0);           
            fread(fragment,sizeof(Mft_Fragment), 1, vfs->file);
            item->fragments[j]=fragment;
        }
        vfs->mft->mft_items.push_back(item);
    }
    vfs->current_path.push_back(vfs->mft->mft_items[0]);
    return vfs;
}
void Functions::Delete_Links(VFS* vfs, Mft_Item* mftItem){
    for (size_t i=0; i<vfs->mft->mft_items.size(); i++) {
        if(vfs->mft->mft_items[i]->is_symlink && (vfs->mft->mft_items[i]->linked_UID == mftItem->uid)){
            vfs->mft->mft_items[i]->linked_UID = -1;
        }
    }
}

Mft_Item* Functions::Get_Mft_Item(VFS* vfs, int uid){
    for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
        if(vfs->mft->mft_items[i]->uid == uid){
            return vfs->mft->mft_items[i];
        }
    }
    return NULL;
}
void  Functions::Print_MFT(VFS* vfs){
	for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
		if(vfs->mft->mft_items[i]->is_symlink){
			std::cout << "*";
		}
		else if(vfs->mft->mft_items[i]->is_directory){
			std::cout << "+";
		}
		else{
			std::cout << "-";
		}
		std::cout << vfs->mft->mft_items[i]->item_name << ": "
			<< "UID: "<< vfs->mft->mft_items[i]->uid << " "
			<< "PUID: "<< vfs->mft->mft_items[i]->parent_ID << " "
			<< "SIZE: "<<vfs->mft->mft_items[i]->item_size << std::endl;
	}
	std::cout << std::endl;
}