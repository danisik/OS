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

class Mft_Fragment {
public:
    Mft_Fragment(int, int32_t, int32_t);
    int32_t fragment_start_address;     //start adresa
    int32_t fragment_count;             //pocet clusteru ve fragmentu
    int bitmap_start_ID;
};


class Mft_Item {
    
public:
    Mft_Item(int, bool, string, long, int, bool, int);
    
    int uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
    int parent_ID;
    bool is_directory;                                   //soubor, nebo adresar
    int item_order;                                  //poradi v MFT pri vice souborech, jinak 1
    int item_order_total;                            //celkovy pocet polozek v MFT
    char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
    long item_size;                                  //velikost souboru v bytech
    bool is_symlink;
    int linked_UID;
    Mft_Fragment* fragments[MFT_FRAGMENTS_COUNT]; //fragmenty souboru
};

class Boot_Record{
public:
    Boot_Record(uint64_t, uint16_t);
    void initBootRecord(long);
	uint64_t Get_Cluster_Count() { return this->cluster_count;};
    char signature[9];              //login autora FS
    char volume_descriptor[251];    //popis vygenerovaného FS
	uint64_t disk_size;              //celkova velikost VFS			bytes_per_sector * absolute_number_of_sectors
    uint16_t cluster_size;           //velikost clusteru			TDrive_Parameters::bytes_per_sector
    uint64_t cluster_count;          //pocet clusteru				TDrive_Parameters::absolute_number_of_sectors
    long mft_start_address;      //adresa pocatku mft				NENI POTREBA
    long bitmap_start_address;   //adresa pocatku bitmapy			NENI POTREBA
    long data_start_address;     //adresa pocatku datovych bloku	NENI POTREBA
    long mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
    // stejne jako   MFT_FRAGMENTS_COUNT
};

class MFT{
    
public:
    MFT();
    bool Is_In_MFT(string);
    vector<Mft_Item*> mft_items;
    int UID_counter;
    int32_t size;
    
};

class VFS{
public:
    VFS(FILE*, uint64_t, uint16_t);
    FILE* file;
    static void Print_Current_Path(VFS*);
    Boot_Record* boot_record;
    MFT* mft;
    bool* bitmap;
    vector<Mft_Item*> current_path;
    
};

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
    static void removeFile(VFS*, string);
    static void Print_MFT(VFS*);
    static void Print_File(VFS*, string);
    static void Move_File(VFS*, string, string);
    static void Copy_File(VFS*, string, string);
    static void Info(VFS*, string);
    static void Export_File(VFS*, string, string);
    static VFS* Format(VFS*, string);
    static void Create_Symlink(VFS*, string, string);
};

class Exist_Item{
public:
    int parent_ID;
    int uid;
    bool exists;
    bool path_exists;
    bool is_directory;
};

class Functions{
public:
    static Exist_Item* Check_Path(VFS*, string);
    static bool Is_Directory_Empty(VFS*, Exist_Item*);
    static void Move_To_Path(VFS*, string);
    static bool Is_Bitmap_Writable(VFS*, long);
    static void Write_To_Data_Block(VFS*, Mft_Item*);
    static void Remove_From_Data_Block(VFS*, Mft_Item*);
    static void Write_To_Clusters(VFS*, Mft_Item*, FILE*);
    static void Copy_To_Clusters(VFS*,Mft_Item*, Mft_Item*);
    static void Save_Vfs_To_File(VFS*);
    static void Print_Bitmap(VFS*);
    static Mft_Item* Get_Mft_Item(VFS*, int);
    static void Print_Clusters(VFS*, Mft_Item*);
    static void Print_MFT(VFS* );
    static void Export_File(VFS*, Mft_Item*, FILE*);
    static VFS* Load_VFS(FILE*);
    static void Delete_Links(VFS*, Mft_Item*);
    static void Print_Boot_Record(VFS*);
    
};



#endif /* Header_h */
