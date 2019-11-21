#include "header.h"

MFT::MFT(){
    this->size = 0;
    this->UID_counter = 0;
    this->mft_items = std::vector<Mft_Item*>();
}

bool MFT::Is_In_MFT(std::string path){
    for (size_t i = 0; i<mft_items.size(); i++) {
        if(std::strcmp(mft_items[i]->item_name, path.c_str())==0){
            return true;
        }
    }
    return false;
}
