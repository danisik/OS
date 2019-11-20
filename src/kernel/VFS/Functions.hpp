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
    static Exist_Item* Check_Path(VFS*, string);
    static bool Is_Directory_Empty(VFS*, Exist_Item*);
    static void Move_To_Path(VFS*, string);
    static bool Is_Bitmap_Writable(VFS*, long);
    static void Write_To_Data_Block(VFS*, Mft_Item*);
    static void Remove_From_Data_Block(VFS*, Mft_Item*);
    static void Write_To_Clusters(VFS*, Mft_Item*, FILE*);
    static void Print_Bitmap(VFS*);
};
