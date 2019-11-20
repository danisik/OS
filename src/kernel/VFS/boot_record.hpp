#ifndef boot_record_hpp
#define boot_record_hpp

#include <stdio.h>
#include <string>
using namespace std;

class Boot_Record{
public:
    Boot_Record(long);
    long Get_Cluster_Count() { return this->cluster_count;};
    char signature[9];              //login autora FS
    char volume_descriptor[251];    //popis vygenerovaného FS
    long disk_size;              //celkova velikost VFS
    long cluster_size;           //velikost clusteru
    long cluster_count;          //pocet clusteru
    long mft_start_address;      //adresa pocatku mft
    long bitmap_start_address;   //adresa pocatku bitmapy
    long data_start_address;     //adresa pocatku datovych bloku
    long mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
    // stejne jako   MFT_FRAGMENTS_COUNT
};

#endif /* boot_record_hpp */
