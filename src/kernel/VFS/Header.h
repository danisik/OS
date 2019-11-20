#ifndef Header_h
#define Header_h
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <math.h>
#include "Constants.hpp"

using namespace std;

class MftFragment {
public:
    MftFragment(int, int32_t, int32_t);
    int32_t fragment_start_address;     //start adresa
    int32_t fragment_count;             //pocet clusteru ve fragmentu
    int bitmapStartID;
};


class MftItem {
    
public:
    MftItem(int, bool, string, long, int, bool, int);
    
    int uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
    int parentID;
    bool isDirectory;                                   //soubor, nebo adresar
    int item_order;                                  //poradi v MFT pri vice souborech, jinak 1
    int item_order_total;                            //celkovy pocet polozek v MFT
    char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
    long item_size;                                  //velikost souboru v bytech
    bool isSymlink;
    int linkedUID;
    MftFragment* fragments[MFT_FRAGMENTS_COUNT]; //fragmenty souboru
};

class BootRecord{
public:
    BootRecord(long);
    void initBootRecord(long);
    long getClusterCount() { return this->clusterCount;};
    char signature[9];              //login autora FS
    char volume_descriptor[251];    //popis vygenerovaného FS
    long diskSize;              //celkova velikost VFS
    long clusterSize;           //velikost clusteru
    long clusterCount;          //pocet clusteru
    long mftStartAddress;      //adresa pocatku mft
    long bitmapStartAddress;   //adresa pocatku bitmapy
    long dataStartAddress;     //adresa pocatku datovych bloku
    long mftMaxFragmentCount; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
    // stejne jako   MFT_FRAGMENTS_COUNT
};

class MFT{
    
public:
    MFT();
    bool isInMFT(string);
    vector<MftItem*> mftItems;
    int UIDcounter;
    int32_t size;
    
};

class VFS{
public:
    VFS(FILE*, long);
    FILE* file;
    static void printCurrentPath(VFS*);
    BootRecord* bootRecord;
    MFT* mft;
    bool* bitmap;
    vector<MftItem*> currentPath;
    
};

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
    static void removeFile(VFS*, string);
    static void printMFT(VFS*);
    static void printFile(VFS*, string);
    static void moveFile(VFS*, string, string);
    static void copyFile(VFS*, string, string);
    static void info(VFS*, string);
    static void exportFile(VFS*, string, string);
    static VFS* format(VFS*, string);
    static void createSymlink(VFS*, string, string);
};

class ExistItem{
public:
    int parentID;
    int uid;
    bool exists;
    bool pathExists;
    bool isDirectory;
};

class Functions{
public:
    static ExistItem* checkPath(VFS*, string);
    static bool isDirectoryEmpty(VFS*, ExistItem*);
    static void moveToPath(VFS*, string);
    static bool isBitmapWritable(VFS*, long);
    static void writeToDataBlock(VFS*, MftItem*);
    static void removeFromDataBlock(VFS*, MftItem*);
    static void writeToClusters(VFS*, MftItem*, FILE*);
    static void copyToClusters(VFS*,MftItem*, MftItem*);
    static void saveVfsToFile(VFS*);
    static void printBitmap(VFS*);
    static MftItem* getMftItem(VFS*, int);
    static void printClusters(VFS*, MftItem*);
    static void printMFT(VFS* );
    static void exportFile(VFS*, MftItem*, FILE*);
    static VFS* loadVFS(FILE*);
    static void deleteLinks(VFS*, MftItem*);
    static void printBootRecord(VFS*);
    
};



#endif /* Header_h */
