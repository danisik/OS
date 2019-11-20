#include "Header.h"



ExistItem* Functions::checkPath(VFS* vfs, string path){
    ExistItem* item = new ExistItem;
    item->uid = vfs->currentPath[vfs->currentPath.size()-1]->uid;
    item->parentID = vfs->currentPath[vfs->currentPath.size()-1]->parentID;
    item->exists = false;
    item->pathExists = false;
    item->isDirectory = true;
    size_t i;
    string tok;
    while((i = path.find(FOLDER_SPLIT)) != string::npos){
        item->pathExists = false;
        tok = path.substr(0, i);
        path = path.substr(i+1);
        for(size_t j = 0; j < vfs->mft->mftItems.size(); j++){
            if((vfs->mft->mftItems[j]->parentID == item->uid)
               &&(strcmp(vfs->mft->mftItems[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft->mftItems[j]->uid;
                item->parentID = vfs->mft->mftItems[j]->parentID;
                item->pathExists = true;
                if(vfs->mft->mftItems[j]->isDirectory){
                    item->pathExists = true;
                    item->isDirectory = true;
                }
                else {
                    item->pathExists = false;
                    item->isDirectory = false;
                    return item;
                }
                break;
            }
        }
        if(!item->pathExists){
            return item;
        }
        
    }
    tok = path.substr(0,i);
    item->pathExists = true;
    for(size_t j = 0; j < vfs->mft->mftItems.size(); j++){
        if((vfs->mft->mftItems[j]->parentID == item->uid)
           &&(strcmp(vfs->mft->mftItems[j]->item_name, tok.c_str())==0)){
            item->uid = vfs->mft->mftItems[j]->uid;
            item->parentID = vfs->mft->mftItems[j]->parentID;
            item->exists = true;
            if(vfs->mft->mftItems[j]->isDirectory) item->isDirectory = true;
            else item->isDirectory = false;
            return item;
        }
    }
    item->exists=false;
    return item;
}

bool Functions::isDirectoryEmpty(VFS* vfs, ExistItem * item){
    for(size_t i = 0; i < vfs->mft->mftItems.size(); i++){
        if(vfs->mft->mftItems[i]->parentID == item->uid){
            return false;
        }
    }
    return true;
}
void Functions::moveToPath(VFS* vfs, string path){
    ExistItem* item = new ExistItem;
    item->uid = vfs->currentPath[vfs->currentPath.size()-1]->uid;
    item->parentID = vfs->currentPath[vfs->currentPath.size()-1]->parentID;
    size_t i;
    string tok;
    while((i = path.find(FOLDER_SPLIT)) != string::npos){
        tok = path.substr(0, i);
        path = path.substr(i+1);
        for(size_t j = 0; j < vfs->mft->mftItems.size(); j++){
            if((vfs->mft->mftItems[j]->parentID == item->uid)
               &&(strcmp(vfs->mft->mftItems[j]->item_name, tok.c_str())==0)){
                item->uid = vfs->mft->mftItems[j]->uid;
                item->parentID = vfs->mft->mftItems[j]->parentID;
                vfs->currentPath.push_back(vfs->mft->mftItems[j]);
                break;
            }
        }
        
    }
    tok = path.substr(0,i);
    for(size_t j = 0; j < vfs->mft->mftItems.size(); j++){
        if((vfs->mft->mftItems[j]->parentID == item->uid)
           &&(strcmp(vfs->mft->mftItems[j]->item_name, tok.c_str())==0)){
            item->uid = vfs->mft->mftItems[j]->uid;
            item->parentID = vfs->mft->mftItems[j]->parentID;
            item->exists = true;
            vfs->currentPath.push_back(vfs->mft->mftItems[j]);
            return;
        }
    }
}

bool Functions::isBitmapWritable(VFS* vfs, long size){
    long neededBitCount = size/vfs->bootRecord->clusterSize;
    if(size % vfs->bootRecord->clusterSize != 0) neededBitCount++;
    long bitCount = 0;
    for(int i = 0; i < vfs->bootRecord->clusterCount; i++){
        if(!vfs->bitmap[i]) bitCount++;
        if(bitCount==neededBitCount) return true;
    }
    return false;
}

void Functions::writeToDataBlock(VFS * vfs, MftItem * mftItem){
    vfs->mft->mftItems.push_back(mftItem);
    long size = mftItem->item_size;
    long neededBitCount = size/vfs->bootRecord->clusterSize;
    if(size % vfs->bootRecord->clusterSize != 0) neededBitCount++;
    long writtenBitCount = 0;
    int fragmentID = 0;
    
    for (int i = 0; i < vfs->bootRecord->clusterCount; i++) {
        if(writtenBitCount == neededBitCount) break;
        
        if(!vfs->bitmap[i]){
            if(mftItem->fragments[fragmentID]->fragment_count<1){
                mftItem->fragments[fragmentID]->bitmapStartID = i;
                mftItem->fragments[fragmentID]->fragment_start_address = vfs->bootRecord->dataStartAddress + i*vfs->bootRecord->clusterSize;
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
void Functions::writeToClusters(VFS * vfs, MftItem* mftItem, FILE* file){
    int fragmentID = 0;
    int filePointer = 0;
    long size = mftItem->item_size;
    char buffer[CLUSTER_SIZE];
    while (mftItem->fragments[fragmentID]->fragment_start_address != 0) {
        for (int j = 0; j<mftItem->fragments[fragmentID]->fragment_count; j++) {
            
            fseek(file, filePointer*vfs->bootRecord->clusterSize, SEEK_SET);
            fread(buffer, vfs->bootRecord->clusterSize, 1, file);
            fseek(vfs->file, mftItem->fragments[fragmentID]->fragment_start_address + j*vfs->bootRecord->clusterSize, SEEK_SET);
            if(size>=vfs->bootRecord->clusterSize){
                fwrite(buffer, vfs->bootRecord->clusterSize, 1, vfs->file);
                size = size - vfs->bootRecord->clusterSize;
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
void Functions::exportFile(VFS * vfs, MftItem * mftItem, FILE* file){
    int fragmentID=0;
    long size = mftItem->item_size;
    char buffer[CLUSTER_SIZE];
    while(mftItem->fragments[fragmentID]->fragment_start_address!=0){
        for (int j = 0; j<mftItem->fragments[fragmentID]->fragment_count; j++) {
            if(size>vfs->bootRecord->clusterSize){
                fseek(vfs->file, mftItem->fragments[fragmentID]->fragment_start_address + j*vfs->bootRecord->clusterSize, SEEK_SET);
                fread(buffer, vfs->bootRecord->clusterSize, 1, vfs->file);
                fwrite(buffer, vfs->bootRecord->clusterSize, 1, file);
                size = size - vfs->bootRecord->clusterSize;
            }
            else{
                fseek(vfs->file, mftItem->fragments[fragmentID]->fragment_start_address + j*vfs->bootRecord->clusterSize, SEEK_SET);
                fread(buffer, size, 1, vfs->file);
                fwrite(buffer, size, 1, file);
                return;
            }
        }
        fragmentID++;
    }
}
void Functions::copyToClusters(VFS* vfs, MftItem* srcFile, MftItem* destFile){
    int fragmentID = 0;
    int filePointer;
    long size = srcFile->item_size;
    int bufferPointer = 0;
    vector<string> bufferVector;
    char buffer[CLUSTER_SIZE];
    while (srcFile->fragments[fragmentID]->fragment_start_address != 0) {
        filePointer = srcFile->fragments[fragmentID]->fragment_start_address;
        for (int j = 0; j<srcFile->fragments[fragmentID]->fragment_count; j++) {
            
            fseek(vfs->file, filePointer+j*vfs->bootRecord->clusterSize, SEEK_SET);
            memset(buffer, 0, vfs->bootRecord->clusterSize);
            fread(buffer, vfs->bootRecord->clusterSize, 1, vfs->file);
            bufferVector.push_back(buffer);
        }
        fragmentID++;
    }
    fragmentID = 0;
    while (destFile->fragments[fragmentID]->fragment_start_address != 0) {
        filePointer = destFile->fragments[fragmentID]->fragment_start_address;
        for (int j = 0; j<destFile->fragments[fragmentID]->fragment_count; j++) {
            memset(buffer, 0, vfs->bootRecord->clusterSize);
            fseek(vfs->file, filePointer + j*vfs->bootRecord->clusterSize, SEEK_SET);
            if(size>=vfs->bootRecord->clusterSize){
                strcpy_s(buffer, sizeof(char*), bufferVector[bufferPointer].data());
                fwrite(buffer, vfs->bootRecord->clusterSize, 1, vfs->file);
                bufferPointer++;
                size = size - vfs->bootRecord->clusterSize;
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
void Functions::printBitmap(VFS * vfs){
    for (int i = 0; i<vfs->bootRecord->clusterCount; i++) {
        if(vfs->bitmap[i]){
            cout << "1";
        }
        else{
            cout <<"0";
        }
    }
    cout << endl <<endl;
}

void Functions::removeFromDataBlock(VFS* vfs, MftItem* mftItem){
    for (int i = 0; i < MFT_FRAGMENTS_COUNT; i++) {
        for (int j = 0; j<mftItem->fragments[i]->fragment_count; j++) {
            vfs->bitmap[mftItem->fragments[i]->bitmapStartID+j] = false;
        }
    }
}

void Functions::saveVfsToFile(VFS* vfs){
    //zapis bootrecordu
    fseek(vfs->file, 0, SEEK_SET);
    fwrite(vfs->bootRecord, sizeof(BootRecord), 1, vfs->file);
    fflush(vfs->file);
    //zapis mft
    fseek(vfs->file, vfs->bootRecord->mftStartAddress, SEEK_SET);
    fwrite(vfs->mft, sizeof(MFT), 1, vfs->file);
    fflush(vfs->file);
    //zapis mftItems
    for (size_t i = 0; i<vfs->mft->mftItems.size(); i++) {
        fseek(vfs->file,vfs->bootRecord->mftStartAddress+sizeof(MFT)+i*sizeof(MftItem)+ i*vfs->bootRecord->mftMaxFragmentCount*sizeof(MftFragment), SEEK_SET);
        fwrite(vfs->mft->mftItems[i], sizeof(MftItem), 1, vfs->file);
        fflush(vfs->file);
        for (int j = 0; j<vfs->bootRecord->mftMaxFragmentCount; j++) {
            fseek(vfs->file, vfs->bootRecord->mftStartAddress+sizeof(MFT)+i*sizeof(MftItem)+i*vfs->bootRecord->mftMaxFragmentCount*sizeof(MftFragment) +sizeof(MftItem)+j*sizeof(MftFragment), SEEK_SET);
            fwrite(vfs->mft->mftItems[i]->fragments[j], sizeof(MftFragment), 1, vfs->file);
            fflush(vfs->file);
        }
    }
    //zapis bitmapy
    fseek(vfs->file, vfs->bootRecord->bitmapStartAddress, SEEK_SET);
    fwrite(vfs->bitmap, sizeof(bool), vfs->bootRecord->clusterCount, vfs->file);
    fflush(vfs->file);
}
void Functions::printClusters(VFS * vfs, MftItem * mftItem){
    int fragmentID = 0;
    int filePointer = 0;
    long size = mftItem->item_size;
    char buffer[CLUSTER_SIZE];
    while (mftItem->fragments[fragmentID]->fragment_start_address != 0) {
        for (int j = 0; j<mftItem->fragments[fragmentID]->fragment_count; j++) {
            
            fseek(vfs->file, mftItem->fragments[fragmentID]->fragment_start_address + j*vfs->bootRecord->clusterSize, SEEK_SET);
            if(size>=vfs->bootRecord->clusterSize){
                fread(buffer, vfs->bootRecord->clusterSize, 1, vfs->file);
                for(int i = 0; i<vfs->bootRecord->clusterSize;i++){
                    cout << buffer[i];
                }
                size = size - vfs->bootRecord->clusterSize;
                filePointer++;
            }
            else{
                memset(buffer, 0, vfs->bootRecord->clusterSize);
                fread(buffer, size, 1, vfs->file);
                for(int i = 0; i<size;i++){
                    cout << buffer[i];
                }
                cout << endl;
                return;
            }
        }
        fragmentID++;
    }
}

VFS* Functions::loadVFS(FILE * file){
    //bootrecord
    BootRecord *bootRecord = new BootRecord(1);
    fseek(file, 0, SEEK_SET);
    fread(bootRecord, sizeof(BootRecord), 1, file);
    VFS* vfs = new VFS(file, bootRecord->diskSize);
    vfs->bootRecord = bootRecord;
    //bitmap
    fseek(vfs->file, vfs->bootRecord->bitmapStartAddress, SEEK_SET);
    fread(vfs->bitmap, sizeof(bool), vfs->bootRecord->getClusterCount(), vfs->file);
    //MFT
    MFT* mft = new MFT();
    fseek(vfs->file, vfs->bootRecord->mftStartAddress, SEEK_SET);
    fread(mft, sizeof(MFT), 1, vfs->file);
    vfs->mft->size = mft->size;
    vfs->mft->UIDcounter = mft->UIDcounter;
    //MFTItems
    for (int i = 0; i<=vfs->mft->size; i++) {
        fseek(vfs->file,vfs->bootRecord->mftStartAddress+sizeof(MFT)+i*sizeof(MftItem)+ i*vfs->bootRecord->mftMaxFragmentCount*sizeof(MftFragment), SEEK_SET);
        MftItem* item = new MftItem(0, false, "", 0, 0, false, 0);
        fread(item, sizeof(MftItem), 1, vfs->file);
        for (int j = 0; j<vfs->bootRecord->mftMaxFragmentCount; j++) {
            fseek(vfs->file, vfs->bootRecord->mftStartAddress+sizeof(MFT)+i*sizeof(MftItem)+i*vfs->bootRecord->mftMaxFragmentCount*sizeof(MftFragment) +sizeof(MftItem)+j*sizeof(MftFragment), SEEK_SET);
            MftFragment * fragment = new MftFragment(0, 0, 0);           
            fread(fragment,sizeof(MftFragment), 1, vfs->file);
            item->fragments[j]=fragment;
        }
        vfs->mft->mftItems.push_back(item);
    }
    vfs->currentPath.push_back(vfs->mft->mftItems[0]);
    return vfs;
}
void Functions::deleteLinks(VFS* vfs, MftItem* mftItem){
    for (size_t i=0; i<vfs->mft->mftItems.size(); i++) {
        if(vfs->mft->mftItems[i]->isSymlink && (vfs->mft->mftItems[i]->linkedUID == mftItem->uid)){
            vfs->mft->mftItems[i]->linkedUID = -1;
        }
    }
}

MftItem* Functions::getMftItem(VFS* vfs, int uid){
    for (size_t i = 0; i<vfs->mft->mftItems.size(); i++) {
        if(vfs->mft->mftItems[i]->uid == uid){
            return vfs->mft->mftItems[i];
        }
    }
    return NULL;
}

void  Functions::printMFT(VFS* vfs){
    for (size_t i = 0; i<vfs->mft->mftItems.size(); i++) {
        if(vfs->mft->mftItems[i]->isSymlink){
            cout << "*";
        }
        else if(vfs->mft->mftItems[i]->isDirectory){
            cout << "+";
        }
        else{
            cout << "-";
        }
        cout << vfs->mft->mftItems[i]->item_name << ": "
        << "UID: "<< vfs->mft->mftItems[i]->uid << " "
        << "PUID: "<< vfs->mft->mftItems[i]->parentID << " "
        << "SIZE: "<<vfs->mft->mftItems[i]->item_size << endl;
    }
    cout << endl;
}
void Functions::printBootRecord(VFS *vfs){
    cout <<"SIGNATURE: " << vfs->bootRecord->signature<<endl
    << "DISK_SIZE: " << vfs->bootRecord->diskSize<<endl
    << "CLUSTER_SIZE: " << vfs->bootRecord->clusterSize<<endl
    << "CLUSTE_COUNT: " << vfs->bootRecord->clusterCount<<endl
    << "MFT_ADRESS: " << vfs->bootRecord->mftStartAddress<<endl
    << "BITMAP_ADRESS: " << vfs->bootRecord->bitmapStartAddress<<endl
    << "DATA_ADRESS: "<< vfs->bootRecord->dataStartAddress<<endl
    << "FRAGMENT_COUNT: "<< vfs->bootRecord->mftMaxFragmentCount<<endl;
}
