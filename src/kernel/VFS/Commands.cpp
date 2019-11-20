#include "Header.h"


void Commands::createDirectory(VFS* vfs, string path){
    ExistItem* item = Functions::checkPath(vfs, path);
    size_t i = path.find_last_of(FOLDER_SPLIT);
    path = path.substr(i+1);

    if(!item->pathExists){
        cout << "PATH NOT FOUND" << endl;
    }
    else if(!item->exists && item->pathExists){
        int size  = 1;
        if(Functions::isBitmapWritable(vfs, size)){
            
            MftItem *mftItem = new MftItem(vfs->mft->UIDcounter, true, path, size, item->uid, false, -1);
            vfs->mft->UIDcounter++;
            vfs->mft->size++;
            Functions::writeToDataBlock(vfs, mftItem);
            Functions::saveVfsToFile(vfs);
        }
    }
    else if(item->exists){
        cout << "FOLDER ALREADY EXISTS"<<endl;
    }
}

void Commands::moveToDirectory(VFS* vfs, string path){
    if(strcmp(path.c_str(), "..")==0){
        if(vfs->currentPath[vfs->currentPath.size()-1]->uid != 0){
            vfs->currentPath.pop_back();
            return;
        }
    }
    ExistItem* item = Functions::checkPath(vfs, path);
        
    if(item->pathExists && item->exists && item->isDirectory){
        Functions::moveToPath(vfs, path);
    }
    else if(!item->exists || !item->pathExists){
        cout << "PATH NOT FOUND" << endl;
    }
        
        /*for(int i = 0; i < vfs->mft->mftItems.size(); i++){
            if((vfs->currentPath[vfs->currentPath.size()-1]->uid == vfs->mft->mftItems[i]->parentID)
               &&(strcmp(vfs->mft->mftItems[i]->item_name, path.c_str())==0)){
                vfs->currentPath.push_back(vfs->mft->mftItems[i]);
            }
        }*/
}
void Commands::moveToRoot(VFS *vfs){
    while(vfs->currentPath[vfs->currentPath.size()-1]->uid != 0){
        vfs->currentPath.pop_back();
    }
}

void Commands::listWithParams(VFS* vfs, string path){
    ExistItem* item = Functions::checkPath(vfs, path);
    if(item->pathExists && item->exists){
        for (size_t i = 0; i<vfs->mft->mftItems.size(); i++) {
            if(vfs->mft->mftItems[i]->parentID == item->uid){
                if(vfs->mft->mftItems[i]->isSymlink){
                    cout << "*" << vfs->mft->mftItems[i]->item_name << endl;
                }
                else if(vfs->mft->mftItems[i]->isDirectory){
                    cout <<"+"<< vfs->mft->mftItems[i]->item_name << endl;
                } else{
                    cout <<"-"<< vfs->mft->mftItems[i]->item_name << endl;
                }
            }
        }
    }
    else if(!item->exists || !item->pathExists){
        cout << "PATH NOT FOUND" << endl;
    }
}
void Commands::list(VFS* vfs){
    for (size_t i = 0; i<vfs->mft->mftItems.size(); i++) {
        if(vfs->mft->mftItems[i]->parentID == vfs->currentPath[vfs->currentPath.size()-1]->uid){
            if(vfs->mft->mftItems[i]->isSymlink){
                cout << "*" << vfs->mft->mftItems[i]->item_name << endl;
            }
            else if(vfs->mft->mftItems[i]->isDirectory){
                cout <<"+"<< vfs->mft->mftItems[i]->item_name << endl;
            } else{
                cout <<"-"<< vfs->mft->mftItems[i]->item_name << endl;
            }
        }
    }
}

void Commands::removeDirectory(VFS * vfs, string path){
    ExistItem* item = Functions::checkPath(vfs, path);
    
    if(item->exists && item->pathExists){
        if(Functions::isDirectoryEmpty(vfs, item)){
            for(size_t i = 0; i < vfs->mft->mftItems.size(); i++){
                if(vfs->mft->mftItems[i]->uid == item->uid){
                    Functions::deleteLinks(vfs, vfs->mft->mftItems[i]);
                    Functions::removeFromDataBlock(vfs, vfs->mft->mftItems[i]);
                    vfs->mft->mftItems.erase(vfs->mft->mftItems.begin() + i);
                    vfs->mft->size--;
                    Functions::saveVfsToFile(vfs);
                }
            }
            //Functions::printBitmap(vfs);
        }
        else {
            cout <<"DIRECTORY NOT EMPTY"<< endl;
        }
    }
    else if(!item->exists || !item->pathExists){
        cout << "DIRECTORY NOT FOUND" << endl;
    }
}
void Commands::removeFile(VFS * vfs, string path){
    ExistItem* item = Functions::checkPath(vfs, path);
    if(item->exists && item->pathExists){
            for(size_t i = 0; i < vfs->mft->mftItems.size(); i++){
                if(vfs->mft->mftItems[i]->uid == item->uid){
                    Functions::deleteLinks(vfs, vfs->mft->mftItems[i]);
                    Functions::removeFromDataBlock(vfs, vfs->mft->mftItems[i]);
                    vfs->mft->mftItems.erase(vfs->mft->mftItems.begin() + i);
                    vfs->mft->size--;
                    Functions::saveVfsToFile(vfs);
            }
            //Functions::printBitmap(vfs);
        }
    }
    else if(!item->exists || !item->pathExists){
        cout << "FILE NOT FOUND" << endl;
    }
}

void Commands::importFile(VFS* vfs, string source, string dest){
	FILE* file;
	fopen_s(&file, source.c_str(), "r");
    size_t i = dest.find_last_of(FOLDER_SPLIT);
    string name = dest.substr(i+1);
    if(file==NULL){
        cout << "FILE NOT FOUND" << endl;
        return;
    }
    ExistItem * existItem = Functions::checkPath(vfs, dest);
    if(existItem->pathExists && !existItem->exists){
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek (file, 0, SEEK_SET);
        if(Functions::isBitmapWritable(vfs, size)){
            MftItem *mftItem = new MftItem(vfs->mft->UIDcounter, false, name, size, existItem->uid, false, -1);
            vfs->mft->UIDcounter++;
            vfs->mft->size++;
            Functions::writeToDataBlock(vfs, mftItem);
            Functions::writeToClusters(vfs, mftItem, file);
            Functions::saveVfsToFile(vfs);
            //Functions::printBitmap(vfs);
        }
        else{
            cout << "NOT ENOUGH SPACE" <<endl;
        }
    }
    else if(!existItem->pathExists){
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
void Commands::exportFile(VFS * vfs, string source, string dest){
	FILE* file;
	fopen_s(&file, dest.c_str(), "w+");
    if(file==NULL){
        cout << "PATH NOT FOUND" << endl;
        fclose(file);
        return;
    }
    ExistItem * existItem = Functions::checkPath(vfs, source);
    if(existItem->pathExists && existItem->exists){
        MftItem * mftItem = Functions::getMftItem(vfs, existItem->uid);
        Functions::exportFile(vfs, mftItem, file);
        fclose(file);
    }
    else if(!existItem->pathExists || !existItem->exists){
        cout<<"FILE NOT FOUND"<<endl;
        return;
    }
}
void Commands::copyFile(VFS * vfs, string source, string dest){
    ExistItem* existItem = Functions::checkPath(vfs, source);
    MftItem * srcFile;
    if(existItem->pathExists && existItem->exists){
        srcFile = Functions::getMftItem(vfs, existItem->uid);
        existItem = Functions::checkPath(vfs, dest);
        if(existItem->pathExists && existItem->exists){
            if(Functions::isBitmapWritable(vfs, srcFile->item_size)){
                MftItem *mftItem = new MftItem(vfs->mft->UIDcounter, srcFile->isDirectory, srcFile->item_name, srcFile->item_size, existItem->uid, srcFile->isSymlink, srcFile->linkedUID);
                vfs->mft->UIDcounter++;
                vfs->mft->size++;
                Functions::writeToDataBlock(vfs, mftItem);
                if(!mftItem->isSymlink) Functions::copyToClusters(vfs, srcFile, mftItem);
                Functions::saveVfsToFile(vfs);
            }
        }
        
    }
}


void Commands::pwd(VFS* vfs){
    for(size_t i = 0; i<vfs->currentPath.size();i++){
        cout << vfs->currentPath[i]->item_name << "/";
    }
    cout << endl;
}
void Commands::printFile(VFS* vfs, string path){
    ExistItem* item = Functions::checkPath(vfs, path);
    
    if(item->pathExists && item->exists){
        MftItem * mftItem = Functions::getMftItem(vfs, item->uid);
        if(mftItem->isSymlink){
            mftItem = Functions::getMftItem(vfs, mftItem->linkedUID);
            if(mftItem == NULL){
                cout << "LINKED FILE DOESN`T EXIST" <<endl;
                return;
            }
        }
        Functions::printClusters(vfs, mftItem);
    }
    
}
void Commands::printMFT(VFS* vfs){
    
    for(size_t i = 0; i<vfs->mft->mftItems.size(); i++){
        cout <<"-" << vfs->mft->mftItems[i]->item_name << endl;
    }
}
void Commands::moveFile(VFS* vfs, string source, string dest){
    ExistItem *item = Functions::checkPath(vfs, source);
    
    if(item->pathExists && item->exists){
        MftItem * mftItem = Functions::getMftItem(vfs, item->uid);
        item = Functions::checkPath(vfs, dest);
        if (item->pathExists && item->exists) {
            mftItem->parentID = item->uid;
            Functions::saveVfsToFile(vfs);
        }else{
            cout<<"PATH NOT FOUND"<<endl;
        }
    }
    else{
        cout << "FILE NOT FOUND"<<endl;
    }
}
void Commands::info(VFS* vfs, string path){
    ExistItem *item = Functions::checkPath(vfs, path);
    if(item->pathExists && item->exists){
        MftItem * mftItem = Functions::getMftItem(vfs, item->uid);
        cout
        << "NAME: " << mftItem->item_name << endl
        << "UID: " << mftItem->uid << endl
        << "PUID: " << mftItem->parentID << endl
        << "SIZE: " << mftItem->item_size << endl;
        if(mftItem->isSymlink){
            if(mftItem->linkedUID != -1){
                MftItem* linked = Functions::getMftItem(vfs, mftItem->linkedUID);
                cout << "SYMLINK ->"<< " " << linked->item_name << endl;
            }
            else{
                cout << "SYMLINK ->"<< " DELETED FILE"<<endl;
            }
        }
        int fragmentCount = 0;
        while (( fragmentCount < MFT_FRAGMENTS_COUNT) && (mftItem->fragments[fragmentCount]->bitmapStartID != 0) ){
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
VFS* Commands::format(VFS* vfs, string command){
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

void Commands::createSymlink(VFS *vfs, string link, string source){
    ExistItem* existItem = Functions::checkPath(vfs, source);
    if(existItem->pathExists && existItem->exists){
        MftItem * sourceItem = Functions::getMftItem(vfs, existItem->uid);
        if(sourceItem->isDirectory){
            cout << "CANNOT LINK DIRECTORY" << endl;
            return;
        }
        existItem = Functions::checkPath(vfs, link);
        if(existItem->pathExists && !existItem->exists){
            size_t i = link.find_last_of(FOLDER_SPLIT);
            link = link.substr(i+1);
            MftItem * linkItem = new MftItem(vfs->mft->UIDcounter, sourceItem->isDirectory, link, 1, existItem->uid, true, sourceItem->uid);
            vfs->mft->mftItems.push_back(linkItem);
            vfs->mft->UIDcounter++;
            vfs->mft->size++;
            Functions::saveVfsToFile(vfs);
        }
    }
    else {
        cout << "FILE NOT FOUND" << endl;
    }
    
}



