#ifndef boot_record_hpp
#define boot_record_hpp

#include <stdio.h>
#include <string>
using namespace std;

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

#endif /* boot_record_hpp */
