#ifndef Functions_hpp
#define Functions_hpp

#include <stdio.h>
#include "VFS.hpp"
#include "existingItem.hpp"
#include "Constants.hpp"

#endif /* Functions_hpp */


using namespace std;


class Functions{
public:
    static ExistItem* checkPath(VFS*, string);
    static bool isDirectoryEmpty(VFS*, ExistItem*);
    static void moveToPath(VFS*, string);
    static bool isBitmapWritable(VFS*, long);
    static void writeToDataBlock(VFS*, MftItem*);
    static void removeFromDataBlock(VFS*, MftItem*);
    static void writeToClusters(VFS*, MftItem*, FILE*);
    static void printBitmap(VFS*);
};
