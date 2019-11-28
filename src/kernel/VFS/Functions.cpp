#include "header.h"

size_t Functions::Create_Item(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path, kiv_os::NFile_Attributes is_directory) {

	Exist_Item* item = Functions::Check_Path(vfs, path, current_path);
	size_t i = path.find_last_of(FOLDER_SPLIT);
	path = path.substr(i + 1);

	if (!item->path_exists) {
		std::cout << "PATH NOT FOUND" << std::endl;
	}
	else if (!item->exists && item->path_exists) {
		int size = 1;
		if (Functions::Is_Bitmap_Writable(vfs, size)) {

			while (vfs->mft_items.find(vfs->mft->UID_counter) != vfs->mft_items.end()) {
				vfs->mft->UID_counter++;
			}

			Mft_Item *mftItem = new Mft_Item(vfs->mft->UID_counter, is_directory, path, size, item->uid);
			vfs->mft->UID_counter++;
			vfs->mft->size++;
			Functions::Write_To_Data_Block(vfs, mftItem);

			Functions::Get_Mft_Item(vfs, item->uid)->item_size++;
			vfs->mft_items.insert(std::pair<size_t, Mft_Item*>(mftItem->uid, mftItem));
			Functions::Save_VFS_MFT(vfs);
			Functions::Save_VFS_MFT_Item(vfs, mftItem->uid);
			return mftItem->uid;
		}
	}
	else if (item->exists) {
		std::cout << "ITEM ALREADY EXISTS" << std::endl;
	}
	return -2;
}

bool Functions::Move_To_Directory(VFS* vfs, std::string path, std::vector<Mft_Item*> &current_path) {
	path.push_back('\0');
	if (strcmp(path.c_str(), ".") == 0) {
		return true;
	}
	if (strcmp(path.c_str(), "..") == 0) {
		if (current_path[current_path.size() - 1]->uid != -1) {
			current_path.pop_back();
			return true;
		}
	}
	Exist_Item* item = Functions::Check_Path(vfs, path, current_path);

	if (item->path_exists && item->exists) {
		if (item->is_directory != kiv_os::NFile_Attributes::Directory) {
			printf("Item is not directory.\n");
			return false;
		}

		Functions::Move_To_Path(vfs, path, current_path);
		return true;
	}
	else if (!item->exists || !item->path_exists) {
		return false;
	}
}

void Functions::Remove_Item(VFS * vfs, std::string path, std::vector<Mft_Item*> &current_path) {
	Exist_Item* item = Functions::Check_Path(vfs, path, current_path);

	if (item->exists && item->path_exists) {
		if (Functions::Is_Directory_Empty(vfs, item)) {
			for (size_t i = 0; i < vfs->mft_items.size(); i++) {
				if (vfs->mft_items[i]->uid == item->uid) {
					vfs->mft_items.erase(item->uid);
					Functions::Remove_From_Data_Block(vfs, vfs->mft_items[item->uid]);

					Mft_Item* last_item = vfs->mft_items.at(vfs->mft_items.size() - 1);
					vfs->mft_items.erase(last_item->uid);
					last_item->uid = item->uid;
					vfs->mft_items.insert(std::pair<size_t, Mft_Item*>(last_item->uid, last_item));


					Functions::Get_Mft_Item(vfs, item->parent_ID)->item_size--;
	
					vfs->mft->size--;
					vfs->mft->UID_counter--;
					Functions::Save_VFS_MFT(vfs);
					Functions::Save_VFS_MFT_Item(vfs, last_item->uid);
				}
			}
			//Functions::printBitmap(vfs);
		}
		else {
			std::cout << "DIRECTORY NOT EMPTY" << std::endl;
		}
	}
	else if (!item->exists || !item->path_exists) {
		std::cout << "ITEM NOT FOUND" << std::endl;
	}
}

Exist_Item* Functions::Check_Path(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path){
    Exist_Item* item = new Exist_Item;
    item->uid = current_path[current_path.size()-1]->uid;
    item->parent_ID = current_path[current_path.size()-1]->parent_ID;
    item->exists = false;
    item->path_exists = false;
    item->is_directory = kiv_os::NFile_Attributes::Directory;
    size_t i;
	std::string tok;
    while((i = path.find(FOLDER_SPLIT)) != std::string::npos){
        item->path_exists = false;
        tok = path.substr(0, i);
        path = path.substr(i+1);
        for(size_t j = 0; j < vfs->mft_items.size(); j++){
            if((vfs->mft_items[j]->parent_ID == item->uid)
               &&(strcmp(vfs->mft_items[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft_items[j]->uid;
                item->parent_ID = vfs->mft_items[j]->parent_ID;
                item->path_exists = true;
                if(vfs->mft_items[j]->is_directory == kiv_os::NFile_Attributes::Directory){
                    item->path_exists = true;
                    item->is_directory = kiv_os::NFile_Attributes::Directory;
                }
                else {
					printf("Item is not a directory.\n");
                    item->path_exists = false;
                    item->is_directory = kiv_os::NFile_Attributes::System_File;
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
    for(size_t j = 0; j < vfs->mft_items.size(); j++){
        if((vfs->mft_items[j]->parent_ID == item->uid)
           &&(strcmp(vfs->mft_items[j]->item_name, tok.c_str())==0)){
            item->uid = vfs->mft_items[j]->uid;
            item->parent_ID = vfs->mft_items[j]->parent_ID;
            item->exists = true;
            if(vfs->mft_items[j]->is_directory == kiv_os::NFile_Attributes::Directory) item->is_directory = kiv_os::NFile_Attributes::Directory;
            else item->is_directory = kiv_os::NFile_Attributes::System_File;
            return item;
        }
    }

	if (path.size() == 0 && item->path_exists == true && !item->exists && item->is_directory == kiv_os::NFile_Attributes::Directory) {
		// Item is actual directory.
		item->parent_ID = current_path.back()->parent_ID;
		item->uid = current_path.back()->uid;
		item->exists = true;
		item->path_exists = true;
		item->is_directory = kiv_os::NFile_Attributes::Directory;
		return item;
	}
    item->exists=false;
    return item;
}

bool Functions::Is_Directory_Empty(VFS* vfs, Exist_Item * item){
	if (item->is_directory != kiv_os::NFile_Attributes::Directory) {
		return true;
	}
    for(size_t i = 0; i < vfs->mft_items.size(); i++){
		// tady to crashovalo TODO
        if(vfs->mft_items[i]->parent_ID == item->uid){
            return false;
        }
    }
    return true;
}
void Functions::Move_To_Path(VFS* vfs, std::string path, std::vector<Mft_Item*> &current_path){
    Exist_Item* item = new Exist_Item;
    item->uid = current_path[current_path.size()-1]->uid;
    item->parent_ID = current_path[current_path.size()-1]->parent_ID;
    size_t i;
	std::string tok;
    while((i = path.find(FOLDER_SPLIT)) != std::string::npos){
        tok = path.substr(0, i);
        path = path.substr(i+1);
        for(size_t j = 0; j < vfs->mft_items.size(); j++){
            if((vfs->mft_items[j]->parent_ID == item->uid)
               &&(strcmp(vfs->mft_items[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft_items[j]->uid;
                item->parent_ID = vfs->mft_items[j]->parent_ID;
				current_path.push_back(vfs->mft_items[j]);
                break;
            }
        }
        
    }
    tok = path.substr(0,i);
    for(size_t j = 0; j < vfs->mft_items.size(); j++){
        if((vfs->mft_items[j]->parent_ID == item->uid)
           &&(strcmp(vfs->mft_items[j]->item_name, tok.c_str())==0)){
            item->uid = vfs->mft_items[j]->uid;
            item->parent_ID = vfs->mft_items[j]->parent_ID;
            item->exists = true;
			current_path.push_back(vfs->mft_items[j]);
            return;
        }
    }
}

bool Functions::Is_Bitmap_Writable(VFS* vfs, size_t size){
	size_t neededBitCount = size/vfs->boot_record->cluster_size;
    if(size % vfs->boot_record->cluster_size != 0) neededBitCount++;
    long bitCount = 0;
    for(int i = 0; i < vfs->boot_record->cluster_count; i++){
        if(!vfs->bitmap[i]) bitCount++;
        if(bitCount==neededBitCount) return true;
    }
    return false;
}

void Functions::Write_To_Data_Block(VFS * vfs, Mft_Item * mftItem){
	size_t size = mftItem->item_size;
	size_t neededBitCount = size/vfs->boot_record->cluster_size;
    if(size % vfs->boot_record->cluster_size != 0) neededBitCount++;
    long writtenBitCount = 0;
    int fragmentID = 0;
    
    for (int i = 0; i < vfs->boot_record->cluster_count; i++) {
        if(writtenBitCount == neededBitCount) break;
        
        if(!vfs->bitmap[i]){
            if(mftItem->fragment_cluster_count[fragmentID] <1){
                mftItem->bitmap_start_ID[fragmentID] = i;
                mftItem->fragment_start_cluster[fragmentID] = vfs->boot_record->data_start_cluster + i;
                mftItem->fragment_cluster_count[fragmentID]++;
                vfs->bitmap[i] = true;
            }
            else{
                mftItem->fragment_cluster_count[fragmentID]++;
                vfs->bitmap[i] = true;
            }
            writtenBitCount++;
        }
        else{
            if(mftItem->fragment_cluster_count[fragmentID] > 0){
                fragmentID++;
            }
        }
    }
}

void Functions::Remove_From_Data_Block(VFS* vfs, Mft_Item* mftItem){
    for (int i = 0; i < MFT_FRAGMENTS_COUNT; i++) {
        for (int j = 0; j<mftItem->fragment_cluster_count[i]; j++) {
            vfs->bitmap[mftItem->bitmap_start_ID[i]+j] = false;
        }
    }
}

Mft_Item* Functions::Get_Mft_Item(VFS* vfs, size_t uid){
    for (size_t i = 0; i<vfs->mft_items.size(); i++) {
        if(vfs->mft_items[i]->uid == uid){
            return vfs->mft_items[i];
        }
    }
    return NULL;
}

std::vector<Mft_Item*> Functions::Get_Items_In_Directory(VFS *vfs, size_t directory_id) {
	std::map<size_t, Mft_Item*> directory_items;

	size_t position = 0;

	for (size_t i = 0; i < vfs->mft_items.size(); i++) {
		if (vfs->mft_items[i]->parent_ID == directory_id) {
			directory_items.insert(std::pair<size_t, Mft_Item*>(position, vfs->mft_items[i]));
			position++;
		}
	}

	for (size_t i = 0; i < directory_items.size(); i++) {
		for (size_t j = 0; j < directory_items.size(); j++) {


			std::string s1 = std::string(directory_items[i]->item_name);
			std::string s2 = std::string(directory_items[j]->item_name);

			if (s1.at(0) < s2.at(0)) {
				Mft_Item *tmp = directory_items[i];
				directory_items[i] = directory_items[j];
				directory_items[j] = tmp;
			}
		}

	}

	std::vector<Mft_Item*> sorted_uid;
	for (size_t i = 0; i < directory_items.size(); i++) {
		sorted_uid.push_back(directory_items[i]);
	}

	return sorted_uid;
}

void Functions::Save_VFS_MFT(VFS* vfs) {
	kiv_hal::TDisk_Address_Packet dap;

	dap.count = 1;
	dap.sectors = static_cast<void*>(vfs->mft);
	dap.lba_index = vfs->boot_record->mft_start_cluster;

	Write_Sectors(vfs->drive_id, dap);
}

void Functions::Save_VFS_MFT_Item(VFS* vfs, size_t uid) {
	kiv_hal::TDisk_Address_Packet dap;

	dap.count = 1;
	dap.sectors = static_cast<void*>(vfs->mft_items[uid]);
	dap.lba_index = vfs->boot_record->mft_start_cluster + uid + 1;

	Write_Sectors(vfs->drive_id, dap);
}

void Functions::Write_Sectors(int drive_id, kiv_hal::TDisk_Address_Packet dap) {
	kiv_hal::TRegisters regs;
	regs.rdx.l = drive_id;
	regs.rax.h = static_cast<decltype(regs.rax.h)>(kiv_hal::NDisk_IO::Write_Sectors);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&dap);

	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
}

void Functions::Read_Sectors(int drive_id, size_t count, size_t lba_index, void* sector) {
	kiv_hal::TDisk_Address_Packet dap;

	dap.count = count;
	dap.sectors = sector;
	dap.lba_index = lba_index;

	kiv_hal::TRegisters regs;
	regs.rdx.l = drive_id;
	regs.rax.h = static_cast<decltype(regs.rax.h)>(kiv_hal::NDisk_IO::Read_Sectors);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&dap);

	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
}

void  Functions::Print_MFT(VFS* vfs) {
	for (size_t i = 0; i < vfs->mft_items.size(); i++) {
		if (vfs->mft_items[i]->is_directory == kiv_os::NFile_Attributes::Directory) {
			std::cout << "+";
		}
		else {
			std::cout << "-";
		}
		std::cout << vfs->mft_items[i]->item_name
			<< ": "
			<< "UID: " << vfs->mft_items[i]->uid
			<< " "
			<< "PUID: " << vfs->mft_items[i]->parent_ID
			<< " "
			<< "SIZE: " << vfs->mft_items[i]->item_size;

		for (size_t j = 0; j < MFT_FRAGMENTS_COUNT; j++) {
			if (vfs->mft_items[i]->fragment_cluster_count[j] == 0) {
				continue;
			}
			else printf("\t fragment %zd is: %zd %zd %zd\n", j, vfs->mft_items[i]->fragment_cluster_count[j], vfs->mft_items[i]->fragment_start_cluster[j], vfs->mft_items[i]->bitmap_start_ID[j]);
		}
	}
	std::cout << std::endl;
}

void Functions::Print_Bitmap(VFS * vfs) {
	for (int i = 0; i < vfs->boot_record->cluster_count; i++) {
		if (vfs->bitmap[i]) {
			std::cout << "1";
		}
		else {
			std::cout << "0";
		}
	}
	std::cout << std::endl << std::endl;
}