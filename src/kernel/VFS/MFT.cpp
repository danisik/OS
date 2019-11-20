#include "Header.h"

MFT::MFT(){
    this->size = 0;
    this->UIDcounter = 0;
    this->mftItems = vector<MftItem*>();
}

bool MFT::isInMFT(string path){
    for (size_t i = 0; i<mftItems.size(); i++) {
        if(std::strcmp(mftItems[i]->item_name, path.c_str())==0){
            return true;
        }
    }
    return false;
}
