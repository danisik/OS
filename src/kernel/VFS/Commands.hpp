#ifndef Commands_hpp
#define Commands_hpp

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include "Functions.hpp"

#endif /* Commands_hpp */
using namespace std;


class Commands{
public:
    static void Create_Directory(VFS*, string);
    static void Move_To_Directory(VFS*, string);
    static void Move_To_Root(VFS*);
    static void Import_File(VFS*, string, string);
    static void List_With_Params(VFS*, string);
    static void List(VFS*);
    static void Pwd(VFS*);
    static void Remove_Directory(VFS*, string);
    static void Print_MFT(VFS*);
};



