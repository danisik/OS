#include "header.h"


size_t Commands::Create_Item(VFS* vfs, std::string path, std::vector<Mft_Item*> current_path, kiv_os::NFile_Attributes is_directory){
	
    Exist_Item* item = Functions::Check_Path(vfs, path, current_path);
    size_t i = path.find_last_of(FOLDER_SPLIT);
    path = path.substr(i+1);

    if(!item->path_exists){
		std::cout << "PATH NOT FOUND" << std::endl;
    }
    else if(!item->exists && item->path_exists){
        int size  = 1;
        if(Functions::Is_Bitmap_Writable(vfs, size)){
            
            Mft_Item *mftItem = new Mft_Item(vfs->mft->UID_counter, is_directory, path, size, item->uid, false, -1);
            vfs->mft->UID_counter++;
            vfs->mft->size++;
            Functions::Write_To_Data_Block(vfs, mftItem);
            //Functions::Save_Vfs_To_File(vfs);

			Functions::Get_Mft_Item(vfs, item->uid)->item_size++;
			return mftItem->uid;
        }
    }
    else if(item->exists){
		std::cout << "ITEM ALREADY EXISTS"<< std::endl;
    }
	return -2;
}

bool Commands::Move_To_Directory(VFS* vfs, std::string path, std::vector<Mft_Item*> &current_path){
	path.push_back('\0');
	if (strcmp(path.c_str(), ".") == 0) {
		return true;
	}
	if(strcmp(path.c_str(), "..")==0){
        if(current_path[current_path.size()-1]->uid != -1){
			current_path.pop_back();
            return true;
        }
    }
    Exist_Item* item = Functions::Check_Path(vfs, path, current_path);
        
    if(item->path_exists && item->exists){
		if (item->is_directory != kiv_os::NFile_Attributes::Directory) {
			printf("Item is not directory.\n");
			return false;
		}

        Functions::Move_To_Path(vfs, path, current_path);
		return true;
    }
    else if(!item->exists || !item->path_exists){
		return false;
    }     
}

void Commands::Move_To_Root(VFS *vfs){
    while(vfs->current_path[vfs->current_path.size()-1]->uid != 0){
        vfs->current_path.pop_back();
    }
}

void Commands::List_With_Params(VFS* vfs, std::string path){
	/*
    Exist_Item* item = Functions::Check_Path(vfs, path);
    if(item->path_exists && item->exists){
        for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
            if(vfs->mft->mft_items[i]->parent_ID == item->uid){
                if(vfs->mft->mft_items[i]->is_symlink){
					std::cout << "*" << vfs->mft->mft_items[i]->item_name << std::endl;
                }
                else if(vfs->mft->mft_items[i]->is_directory){
					std::cout <<"+"<< vfs->mft->mft_items[i]->item_name << std::endl;
                } else{
					std::cout <<"-"<< vfs->mft->mft_items[i]->item_name << std::endl;
                }
            }
        }
    }
    else if(!item->exists || !item->path_exists){
		std::cout << "PATH NOT FOUND" << std::endl;
    }
	*/
}
void Commands::List(VFS* vfs){
    for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
        if(vfs->mft->mft_items[i]->parent_ID == vfs->current_path[vfs->current_path.size()-1]->uid){
            if(vfs->mft->mft_items[i]->is_symlink){
				std::cout << "*" << vfs->mft->mft_items[i]->item_name << std::endl;
            }
            else if(vfs->mft->mft_items[i]->is_directory == kiv_os::NFile_Attributes::Directory){
				std::cout <<"+"<< vfs->mft->mft_items[i]->item_name << std::endl;
            } else{
				std::cout <<"-"<< vfs->mft->mft_items[i]->item_name << std::endl;
            }
        }
    }
}

void Commands::Remove_Item(VFS * vfs, std::string path, std::vector<Mft_Item*> &current_path){
    Exist_Item* item = Functions::Check_Path(vfs, path, current_path);
    
    if(item->exists && item->path_exists){
        if(Functions::Is_Directory_Empty(vfs, item)){
            for(size_t i = 0; i < vfs->mft->mft_items.size(); i++){
                if(vfs->mft->mft_items[i]->uid == item->uid){
                    Functions::Remove_From_Data_Block(vfs, vfs->mft->mft_items[i]);
                    vfs->mft->mft_items.erase(vfs->mft->mft_items.begin() + i);
                    vfs->mft->size--;
					Functions::Get_Mft_Item(vfs, item->uid)->item_size--;
                    //Functions::Save_Vfs_To_File(vfs);
                }
            }
            //Functions::printBitmap(vfs);
        }
        else {
			std::cout <<"DIRECTORY NOT EMPTY"<< std::endl;
        }
    }
    else if(!item->exists || !item->path_exists){
		std::cout << "ITEM NOT FOUND" << std::endl;
    }
}
void Commands::removeFile(VFS * vfs, std::string path){
	/*
    Exist_Item* item = Functions::Check_Path(vfs, path);
    if(item->exists && item->path_exists){
            for(size_t i = 0; i < vfs->mft->mft_items.size(); i++){
                if(vfs->mft->mft_items[i]->uid == item->uid){
                    Functions::Delete_Links(vfs, vfs->mft->mft_items[i]);
                    Functions::Remove_From_Data_Block(vfs, vfs->mft->mft_items[i]);
                    vfs->mft->mft_items.erase(vfs->mft->mft_items.begin() + i);
                    vfs->mft->size--;
                    Functions::Save_Vfs_To_File(vfs);
            }
            //Functions::printBitmap(vfs);
        }
    }
    else if(!item->exists || !item->path_exists){
		std::cout << "FILE NOT FOUND" << std::endl;
    }
	*/
}

void Commands::Copy_File(VFS * vfs, std::string source, std::string dest){
	/*
    Exist_Item* existItem = Functions::Check_Path(vfs, source);
    Mft_Item * srcFile;
    if(existItem->path_exists && existItem->exists){
        srcFile = Functions::Get_Mft_Item(vfs, existItem->uid);
        existItem = Functions::Check_Path(vfs, dest);
        if(existItem->path_exists && existItem->exists){
            if(Functions::Is_Bitmap_Writable(vfs, srcFile->item_size)){
                Mft_Item *mftItem = new Mft_Item(vfs->mft->UID_counter, srcFile->is_directory, srcFile->item_name, srcFile->item_size, existItem->uid, srcFile->is_symlink, srcFile->linked_UID);
                vfs->mft->UID_counter++;
                vfs->mft->size++;
                Functions::Write_To_Data_Block(vfs, mftItem);
                if(!mftItem->is_symlink) Functions::Copy_To_Clusters(vfs, srcFile, mftItem);
                Functions::Save_Vfs_To_File(vfs);
            }
        }
        
    }
	*/
}

void Commands::Print_File(VFS* vfs, std::string path){
	/*
    Exist_Item* item = Functions::Check_Path(vfs, path);
    
    if(item->path_exists && item->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, item->uid);
        if(mftItem->is_symlink){
            mftItem = Functions::Get_Mft_Item(vfs, mftItem->linked_UID);
            if(mftItem == NULL){
				std::cout << "LINKED FILE DOESN`T EXIST" << std::endl;
                return;
            }
        }
        Functions::Print_Clusters(vfs, mftItem);
    }
	*/
    
}
void Commands::Print_MFT(VFS* vfs){

	for(size_t i = 0; i<vfs->mft->mft_items.size(); i++){
		std::cout <<"-" << vfs->mft->mft_items[i]->item_name << std::endl;
	}
}
void Commands::Move_File(VFS* vfs, std::string source, std::string dest){
	/*
    Exist_Item *item = Functions::Check_Path(vfs, source);
    
    if(item->path_exists && item->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, item->uid);
        item = Functions::Check_Path(vfs, dest);
        if (item->path_exists && item->exists) {
            mftItem->parent_ID = item->uid;
            Functions::Save_Vfs_To_File(vfs);
        }else{
			std::cout<<"PATH NOT FOUND"<< std::endl;
        }
    }
    else{
		std::cout << "FILE NOT FOUND"<< std::endl;
    }
	*/
}
void Commands::Info(VFS* vfs, std::string path){
	/*
    Exist_Item *item = Functions::Check_Path(vfs, path);
    if(item->path_exists && item->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, item->uid);
		std::cout
        << "NAME: " << mftItem->item_name << std::endl
        << "UID: " << mftItem->uid << std::endl
        << "PUID: " << mftItem->parent_ID << std::endl
        << "SIZE: " << mftItem->item_size << std::endl;
        if(mftItem->is_symlink){
            if(mftItem->linked_UID != -1){
                Mft_Item* linked = Functions::Get_Mft_Item(vfs, mftItem->linked_UID);
				std::cout << "SYMLINK ->"<< " " << linked->item_name << std::endl;
            }
            else{
				std::cout << "SYMLINK ->"<< " DELETED FILE"<< std::endl;
            }
        }
        int fragmentCount = 0;
        while (( fragmentCount < MFT_FRAGMENTS_COUNT) && (mftItem->fragments[fragmentCount]->bitmap_start_ID != 0) ){
            fragmentCount++;
        }
		std::cout << "FRAGMENTS:" << fragmentCount << std::endl;
        
        for (int i = 0; i < fragmentCount; i++) {
			std::cout << "Fragment_" << i << std::endl
            <<"      Cluster_start_adress: "
            << mftItem->fragments[i]->fragment_start_address<< std::endl
            <<"      Number of clusters: " << mftItem->fragments[i]->fragment_count<< std::endl;
        }
    }
	*/
}



