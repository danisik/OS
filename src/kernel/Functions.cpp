#include "header.h"

size_t Functions::Create_Item(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path, kiv_os::NFile_Attributes is_directory) {
	Exist_Item* item = Functions::Check_Path(vfs, path, current_path);
	size_t i = path.find_last_of(FOLDER_SPLIT);
	path = path.substr(i + 1);

	vfs->mft->size = vfs->mft_items.size();
	vfs->mft->UID_counter = vfs->mft_items.size();

	if (!item->path_exists) {
		printf("Path not found.\n");
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
		printf("Item already exists.\n");
	}
	return 0;
}

bool Functions::Move_To_Directory(VFS* vfs, std::string path, std::vector<Mft_Item*> &current_path) {
	if (path.size() == 0) {
		for (size_t i = 0; i < current_path.size(); i++) {
			printf(current_path[i]->item_name);
			if (i < current_path.size() - 1) {
				printf("\\");
			}
		}
		printf("\n");
		return true;
	}

	path.push_back('\0');

	if (strcmp(path.c_str(), ".") == 0) {
		return true;
	}
	// Convert '\' to '/'.
	if (path.find("\\") != std::string::npos) {
		size_t pos;
		while ((pos = path.find("\\")) != std::string::npos) {
			path.replace(pos, 1, "/");
		}
	}

	// Remove '/' from string if it is in last position.
	if (path[path.size() - 2] == '/') {
		path = path.substr(0, path.size() - 1);
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
	else {
		return false;
	}
}

Mft_Item* Get_Last_Item(VFS *vfs) {
	if (!vfs->mft_items.empty()) {
		return (--vfs->mft_items.end())->second;
	}
	return nullptr;
}

void Functions::Update_Parent_ID(VFS *vfs, size_t old_id, size_t new_id) {
	for (size_t i = 0; i < vfs->mft_items.size(); i++) {
		if (vfs->mft_items[i]->parent_ID == old_id) {
			vfs->mft_items[i]->parent_ID = new_id;
		}
	}
}

void Functions::Remove_Item(VFS * vfs, std::string path, std::vector<Mft_Item*> &current_path) {
	vfs->mft->size = vfs->mft_items.size();
	vfs->mft->UID_counter = vfs->mft_items.size();
	Exist_Item* item = Functions::Check_Path(vfs, path, current_path);
	if (item->exists && item->path_exists) {
		if (Functions::Is_Directory_Empty(vfs, item)) {
			for (size_t i = 0; i < vfs->mft_items.size(); i++) {
				if (vfs->mft_items[i]->uid == item->uid) {
					if (item->uid == 0) {
						printf("Item is root directory.\n");
						return;
					}

					Functions::Remove_From_Data_Block(vfs, vfs->mft_items[item->uid]);
					vfs->mft_items.erase(item->uid);

					if (vfs->mft->size >= 2) {						
						Mft_Item* last_item = Get_Last_Item(vfs);

						if (last_item->uid > item->uid) {
							if (last_item->is_directory == kiv_os::NFile_Attributes::Directory) Update_Parent_ID(vfs, last_item->uid, item->uid);
							vfs->mft_items.erase(last_item->uid);
							last_item->uid = item->uid;
							vfs->mft_items.insert(std::pair<size_t, Mft_Item*>(last_item->uid, last_item));
							Functions::Save_VFS_MFT_Item(vfs, last_item->uid);
						}
					}
					vfs->mft->size--;
					vfs->mft->UID_counter--;
					Functions::Get_Mft_Item(vfs, item->parent_ID)->item_size--;

					Functions::Save_VFS_MFT(vfs);
					return;
				}
			}
		}
		else {
			printf("Directory not empty.\n");			
		}
	}
	else if (!item->exists || !item->path_exists) {
		printf("Item not found.\n");
	}
}

Exist_Item* Functions::Check_Path(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path){
    Exist_Item* item = new Exist_Item;
    item->uid = current_path[current_path.size()-1]->uid;
    item->parent_ID = current_path[current_path.size()-1]->parent_ID;
    item->exists = false;
    item->path_exists = false;
    item->is_directory = kiv_os::NFile_Attributes::Directory;
    size_t i = 0;
	std::string tok;

	size_t j = 0;
	std::string path_tmp = path;
	std::string tok_tmp;
	size_t jump_back = 0;

	if (strcmp(path.c_str(), "../") == 0 || strcmp(path.c_str(), "..") == 0) {
		item->is_directory = kiv_os::NFile_Attributes::Directory;
		item->exists = true;
		item->path_exists = true;
		return item;
	}

	while ((j = path_tmp.find(FOLDER_SPLIT)) != std::string::npos) {
		tok_tmp = path_tmp.substr(0, j);
		path_tmp = path_tmp.substr(j + 1);
		if (strcmp(tok_tmp.c_str(), "..") == 0) {
			jump_back++;
			item->uid = current_path[current_path.size() - jump_back - 1]->uid;
		}
	}

	if (jump_back >= current_path.size()) {
		item->path_exists = false;
		item->exists = false;
		return item;
	}

    while((i = path.find(FOLDER_SPLIT)) != std::string::npos){
        item->path_exists = false;
        tok = path.substr(0, i);
        path = path.substr(i+1);

		if (jump_back > 0) {
			jump_back--;
			continue;
		}

        for(j = 0; j < vfs->mft_items.size(); j++) {
            if((vfs->mft_items[j]->parent_ID == item->uid) &&(strcmp(vfs->mft_items[j]->item_name, tok.c_str())==0)){
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
	const char *name = tok.c_str();

    for(j = 0; j < vfs->mft_items.size(); j++){
        if((vfs->mft_items[j]->parent_ID == item->uid) &&(strcmp(vfs->mft_items[j]->item_name, name)==0)){
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
    for(size_t i = 1; i < vfs->mft_items.size(); i++){
		// tady to crashovalo pøi remove TODO
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

	if (strcmp(path.c_str(), "../") == 0 || strcmp(path.c_str(), "..") == 0) {
		if (current_path.size() > 1) current_path.pop_back();
		return;
	}

	size_t j;
	std::string path_tmp = path;
	std::string tok_tmp;
	size_t jump_back = 0;

	while ((j = path_tmp.find(FOLDER_SPLIT)) != std::string::npos) {
		tok_tmp = path_tmp.substr(0, j);
		path_tmp = path_tmp.substr(j + 1);
		if (strcmp(tok_tmp.c_str(), "..") == 0) {
			jump_back++;
		}
	}

    while((i = path.find(FOLDER_SPLIT)) != std::string::npos){
        tok = path.substr(0, i);
        path = path.substr(i+1);

		if (jump_back > 0) {			
			current_path.pop_back();
			item->uid = current_path[current_path.size() - 1]->uid;
			jump_back--;
			continue;
		}

        for(j = 0; j < vfs->mft_items.size(); j++){
            if((vfs->mft_items[j]->parent_ID == item->uid) &&(strcmp(vfs->mft_items[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft_items[j]->uid;
                item->parent_ID = vfs->mft_items[j]->parent_ID;
				current_path.push_back(vfs->mft_items[j]);
                break;
            }
        }
        
    }
    tok = path.substr(0,i);
    for(j = 0; j < vfs->mft_items.size(); j++){
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
        for (size_t j = 0; j<mftItem->fragment_cluster_count[i]; j++) {
            vfs->bitmap[mftItem->bitmap_start_ID[i]+j] = false;
			mftItem->fragment_cluster_count[i] = 0;
			mftItem->fragment_start_cluster[i] = 0;
			mftItem->bitmap_start_ID[i] = 0;
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
	Process_Sectors(kiv_hal::NDisk_IO::Write_Sectors, vfs->drive_id, 1, vfs->boot_record->mft_start_cluster, static_cast<void*>(vfs->mft));
}

void Functions::Save_VFS_MFT_Item(VFS* vfs, size_t uid) {
	Process_Sectors(kiv_hal::NDisk_IO::Write_Sectors, vfs->drive_id, 1, vfs->boot_record->mft_start_cluster + uid + 1, static_cast<void*>(vfs->mft_items[uid]));
}

void Functions::Process_Sectors(kiv_hal::NDisk_IO operation, int drive_id, size_t count, size_t lba_index, void* sector) {
	kiv_hal::TDisk_Address_Packet dap;

	dap.count = count;
	dap.sectors = sector;
	dap.lba_index = lba_index;

	kiv_hal::TRegisters regs;
	regs.rdx.l = drive_id;
	regs.rax.h = static_cast<decltype(regs.rax.h)>(operation);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&dap);

	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
}