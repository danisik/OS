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
    static void createDirectory(VFS*, string);
    static void moveToDirectory(VFS*, string);
    static void moveToRoot(VFS*);
    static void importFile(VFS*, string, string);
    static void listWithParams(VFS*, string);
    static void list(VFS*);
    static void pwd(VFS*);
    static void removeDirectory(VFS*, string);
    static void printMFT(VFS*);
};



