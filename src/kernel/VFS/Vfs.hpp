#ifndef VFS_hpp
#define VFS_hpp

#include <stdio.h>
#include <iostream>
#include <iterator>
#include <vector>
#include "Constants.hpp"
#include "boot_record.hpp"
#include "MFT.hpp"

using namespace std;

class VFS{
public:
    VFS(FILE*, long);
    FILE* file;
    static void Print_Current_Path(VFS*);
    static void saveVfsToFile(VFS*);
    Boot_Record* boot_record;
    MFT* mft;
    bool* bitmap;
    vector<Mft_Item*> current_path;

};

#endif /* VFS_hpp */
