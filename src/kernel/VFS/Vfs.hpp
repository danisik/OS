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
    static void printCurrentPath(VFS*);
    static void saveVfsToFile(VFS*);
    BootRecord* bootRecord;
    MFT* mft;
    bool* bitmap;
    vector<MftItem*> currentPath;

};

#endif /* VFS_hpp */
