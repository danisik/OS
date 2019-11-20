#include "Header.h"


void Commands::Create_Directory(VFS* vfs, string path){
    Exist_Item* item = Functions::Check_Path(vfs, path);
    size_t i = path.find_last_of(FOLDER_SPLIT);
    path = path.substr(i+1);

    if(!item->path_exists){
        cout << "PATH NOT FOUND" << endl;
    }
    else if(!item->exists && item->path_exists){
        int size  = 1;
        if(Functions::Is_Bitmap_Writable(vfs, size)){
            
            Mft_Item *mftItem = new Mft_Item(vfs->mft->UID_counter, true, path, size, item->uid, false, -1);
            vfs->mft->UID_counter++;
            vfs->mft->size++;
            Functions::Write_To_Data_Block(vfs, mftItem);
            Functions::Save_Vfs_To_File(vfs);
        }
    }
    else if(item->exists){
        cout << "FOLDER ALREADY EXISTS"<<endl;
    }
}

void Commands::Move_To_Directory(VFS* vfs, string path){
    if(strcmp(path.c_str(), "..")==0){
        if(vfs->current_path[vfs->current_path.size()-1]->uid != 0){
            vfs->current_path.pop_back();
            return;
        }
    }
    Exist_Item* item = Functions::Check_Path(vfs, path);
        
    if(item->path_exists && item->exists && item->is_directory){
        Functions::Move_To_Path(vfs, path);
    }
    else if(!item->exists || !item->path_exists){
        cout << "PATH NOT FOUND" << endl;
    }     
}

void Commands::Move_To_Root(VFS *vfs){
    while(vfs->current_path[vfs->current_path.size()-1]->uid != 0){
        vfs->current_path.pop_back();
    }
}

void Commands::List_With_Params(VFS* vfs, string path){
    Exist_Item* item = Functions::Check_Path(vfs, path);
    if(item->path_exists && item->exists){
        for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
            if(vfs->mft->mft_items[i]->parent_ID == item->uid){
                if(vfs->mft->mft_items[i]->is_symlink){
                    cout << "*" << vfs->mft->mft_items[i]->item_name << endl;
                }
                else if(vfs->mft->mft_items[i]->is_directory){
                    cout <<"+"<< vfs->mft->mft_items[i]->item_name << endl;
                } else{
                    cout <<"-"<< vfs->mft->mft_items[i]->item_name << endl;
                }
            }
        }
    }
    else if(!item->exists || !item->path_exists){
        cout << "PATH NOT FOUND" << endl;
    }
}
void Commands::List(VFS* vfs){
    for (size_t i = 0; i<vfs->mft->mft_items.size(); i++) {
        if(vfs->mft->mft_items[i]->parent_ID == vfs->current_path[vfs->current_path.size()-1]->uid){
            if(vfs->mft->mft_items[i]->is_symlink){
                cout << "*" << vfs->mft->mft_items[i]->item_name << endl;
            }
            else if(vfs->mft->mft_items[i]->is_directory){
                cout <<"+"<< vfs->mft->mft_items[i]->item_name << endl;
            } else{
                cout <<"-"<< vfs->mft->mft_items[i]->item_name << endl;
            }
        }
    }
}

void Commands::Remove_Directory(VFS * vfs, string path){
    Exist_Item* item = Functions::Check_Path(vfs, path);
    
    if(item->exists && item->path_exists){
        if(Functions::Is_Directory_Empty(vfs, item)){
            for(size_t i = 0; i < vfs->mft->mft_items.size(); i++){
                if(vfs->mft->mft_items[i]->uid == item->uid){
                    Functions::Delete_Links(vfs, vfs->mft->mft_items[i]);
                    Functions::Remove_From_Data_Block(vfs, vfs->mft->mft_items[i]);
                    vfs->mft->mft_items.erase(vfs->mft->mft_items.begin() + i);
                    vfs->mft->size--;
                    Functions::Save_Vfs_To_File(vfs);
                }
            }
            //Functions::printBitmap(vfs);
        }
        else {
            cout <<"DIRECTORY NOT EMPTY"<< endl;
        }
    }
    else if(!item->exists || !item->path_exists){
        cout << "DIRECTORY NOT FOUND" << endl;
    }
}
void Commands::removeFile(VFS * vfs, string path){
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
        cout << "FILE NOT FOUND" << endl;
    }
}

void Commands::Import_File(VFS* vfs, string source, string dest){
	FILE* file;
	fopen_s(&file, source.c_str(), "r");
    size_t i = dest.find_last_of(FOLDER_SPLIT);
    string name = dest.substr(i+1);
    if(file==NULL){
        cout << "FILE NOT FOUND" << endl;
        return;
    }
    Exist_Item * existItem = Functions::Check_Path(vfs, dest);
    if(existItem->path_exists && !existItem->exists){
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek (file, 0, SEEK_SET);
        if(Functions::Is_Bitmap_Writable(vfs, size)){
            Mft_Item *mftItem = new Mft_Item(vfs->mft->UID_counter, false, name, size, existItem->uid, false, -1);
            vfs->mft->UID_counter++;
            vfs->mft->size++;
            Functions::Write_To_Data_Block(vfs, mftItem);
            Functions::Write_To_Clusters(vfs, mftItem, file);
            Functions::Save_Vfs_To_File(vfs);
            //Functions::printBitmap(vfs);
        }
        else{
            cout << "NOT ENOUGH SPACE" <<endl;
        }
    }
    else if(!existItem->path_exists){
        cout<<"PATH NOT FOUND"<<endl;
        fclose(file);
        return;
    }
    else if(existItem->exists){
        cout<<"FILE ALREADY EXISTS"<<endl;
        fclose(file);
        return;
    }
}
void Commands::Export_File(VFS * vfs, string source, string dest){
	FILE* file;
	fopen_s(&file, dest.c_str(), "w+");
    if(file==NULL){
        cout << "PATH NOT FOUND" << endl;
        fclose(file);
        return;
    }
    Exist_Item * existItem = Functions::Check_Path(vfs, source);
    if(existItem->path_exists && existItem->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, existItem->uid);
        Functions::Export_File(vfs, mftItem, file);
        fclose(file);
    }
    else if(!existItem->path_exists || !existItem->exists){
        cout<<"FILE NOT FOUND"<<endl;
        return;
    }
}
void Commands::Copy_File(VFS * vfs, string source, string dest){
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
}


void Commands::Pwd(VFS* vfs){
    for(size_t i = 0; i<vfs->current_path.size();i++){
        cout << vfs->current_path[i]->item_name << "/";
    }
    cout << endl;
}
void Commands::Print_File(VFS* vfs, string path){
    Exist_Item* item = Functions::Check_Path(vfs, path);
    
    if(item->path_exists && item->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, item->uid);
        if(mftItem->is_symlink){
            mftItem = Functions::Get_Mft_Item(vfs, mftItem->linked_UID);
            if(mftItem == NULL){
                cout << "LINKED FILE DOESN`T EXIST" <<endl;
                return;
            }
        }
        Functions::Print_Clusters(vfs, mftItem);
    }
    
}
void Commands::Print_MFT(VFS* vfs){
    
    for(size_t i = 0; i<vfs->mft->mft_items.size(); i++){
        cout <<"-" << vfs->mft->mft_items[i]->item_name << endl;
    }
}
void Commands::Move_File(VFS* vfs, string source, string dest){
    Exist_Item *item = Functions::Check_Path(vfs, source);
    
    if(item->path_exists && item->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, item->uid);
        item = Functions::Check_Path(vfs, dest);
        if (item->path_exists && item->exists) {
            mftItem->parent_ID = item->uid;
            Functions::Save_Vfs_To_File(vfs);
        }else{
            cout<<"PATH NOT FOUND"<<endl;
        }
    }
    else{
        cout << "FILE NOT FOUND"<<endl;
    }
}
void Commands::Info(VFS* vfs, string path){
    Exist_Item *item = Functions::Check_Path(vfs, path);
    if(item->path_exists && item->exists){
        Mft_Item * mftItem = Functions::Get_Mft_Item(vfs, item->uid);
        cout
        << "NAME: " << mftItem->item_name << endl
        << "UID: " << mftItem->uid << endl
        << "PUID: " << mftItem->parent_ID << endl
        << "SIZE: " << mftItem->item_size << endl;
        if(mftItem->is_symlink){
            if(mftItem->linked_UID != -1){
                Mft_Item* linked = Functions::Get_Mft_Item(vfs, mftItem->linked_UID);
                cout << "SYMLINK ->"<< " " << linked->item_name << endl;
            }
            else{
                cout << "SYMLINK ->"<< " DELETED FILE"<<endl;
            }
        }
        int fragmentCount = 0;
        while (( fragmentCount < MFT_FRAGMENTS_COUNT) && (mftItem->fragments[fragmentCount]->bitmap_start_ID != 0) ){
            fragmentCount++;
        }
        cout << "FRAGMENTS:" << fragmentCount << endl;
        
        for (int i = 0; i < fragmentCount; i++) {
            cout << "Fragment_" << i << endl
            <<"      Cluster_start_adress: "
            << mftItem->fragments[i]->fragment_start_address<<endl
            <<"      Number of clusters: " << mftItem->fragments[i]->fragment_count<<endl;
        }
    }
}
VFS* Commands::Format(VFS* vfs, string command){
    size_t point = command.find(SPLIT);
    string tok = command.substr(0, point);
    string type = command.substr(point + 1);
    long size = 0;
    if(strcmp(type.c_str(), "b")==0){
        size = stol(tok.data());
    }
    else if(strcmp(type.c_str(), "kb")==0){
        size = (long)(stol(tok.data())*pow(2, 10));
    }
    else if(strcmp(type.c_str(), "mb")==0){
        size = (long)(stol(tok.data())*pow(2, 20));
    }
    else{
        size = 0;
    }
    cout << size << endl;
    return new VFS(vfs->file, size);
}

void Commands::Create_Symlink(VFS *vfs, string link, string source){
    Exist_Item* existItem = Functions::Check_Path(vfs, source);
    if(existItem->path_exists && existItem->exists){
        Mft_Item * sourceItem = Functions::Get_Mft_Item(vfs, existItem->uid);
        if(sourceItem->is_directory){
            cout << "CANNOT LINK DIRECTORY" << endl;
            return;
        }
        existItem = Functions::Check_Path(vfs, link);
        if(existItem->path_exists && !existItem->exists){
            size_t i = link.find_last_of(FOLDER_SPLIT);
            link = link.substr(i+1);
            Mft_Item * linkItem = new Mft_Item(vfs->mft->UID_counter, sourceItem->is_directory, link, 1, existItem->uid, true, sourceItem->uid);
            vfs->mft->mft_items.push_back(linkItem);
            vfs->mft->UID_counter++;
            vfs->mft->size++;
            Functions::Save_Vfs_To_File(vfs);
        }
    }
    else {
        cout << "FILE NOT FOUND" << endl;
    }
    
}



