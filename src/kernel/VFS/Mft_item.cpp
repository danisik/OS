#include "Header.h"

MftItem::MftItem(int uid, bool isDirectory, string name, long item_size, int parentID, bool isSymlink, int linkedUID){
    
    this->uid = uid;
    this->isDirectory = isDirectory;
    strcpy_s(this->item_name, 12, name.c_str());
    this->item_order = 1;
    this->item_order_total = 1;
    this->item_size = item_size;
    this->parentID = parentID;
    this->isSymlink = isSymlink;
    this->linkedUID = linkedUID;
    for (int i = 0; i < MFT_FRAGMENTS_COUNT; i++) {
        this->fragments[i] = new MftFragment(0, 0, 0);
    }
    
    
}
