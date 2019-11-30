#ifndef Header_h
#define Header_h
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <math.h>
#include <map>
#include "Constants.hpp"
#include "../../api/hal.h"
#include "../../api/api.h"

class Mft_Fragment {
public:
    Mft_Fragment(int, size_t, size_t);
    size_t fragment_start_cluster;			//start adresa
    size_t fragment_cluster_count;             //pocet clusteru ve fragmentu
    int bitmap_start_ID;
};


class Mft_Item {
    
public:
    Mft_Item(size_t, kiv_os::NFile_Attributes, std::string, size_t, size_t);
    
	size_t uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
	size_t parent_ID;
	kiv_os::NFile_Attributes is_directory;                                   //soubor, nebo adresar
    int item_order;                                  //poradi v MFT pri vice souborech, jinak 1
    int item_order_total;                            //celkovy pocet polozek v MFT
    char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
	size_t item_size;                                  //velikost souboru v bytech
    //Mft_Fragment* fragments[MFT_FRAGMENTS_COUNT]; //fragmenty souboru
	size_t fragment_start_cluster[MFT_FRAGMENTS_COUNT];			//start adresa
	size_t fragment_cluster_count[MFT_FRAGMENTS_COUNT];             //pocet clusteru ve fragmentu
	int bitmap_start_ID[MFT_FRAGMENTS_COUNT];

};

class Boot_Record{
public:
    Boot_Record(uint64_t, uint16_t);
	uint64_t Get_Cluster_Count() { return this->cluster_count;};
    char signature[9];              //login autora FS
    char volume_descriptor[251];    //popis vygenerovaného FS
	size_t disk_size;              //celkova velikost VFS			bytes_per_sector * absolute_number_of_sectors
    uint16_t cluster_size;           //velikost clusteru			TDrive_Parameters::bytes_per_sector
    uint64_t cluster_count;          //pocet clusteru				TDrive_Parameters::absolute_number_of_sectors
    size_t mft_start_cluster;      //cluster pocatku mft				
	size_t bitmap_start_cluster;   //cluster pocatku bitmapy			
	size_t data_start_cluster;     //cluster pocatku datovych bloku	
    long mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
    // stejne jako   MFT_FRAGMENTS_COUNT
};

class MFT{
    
public:
    MFT();
	size_t UID_counter;
	size_t size;
    
};

class VFS{
public:
    VFS(uint64_t, uint16_t, int);
    FILE* file;
    Boot_Record* boot_record;
    MFT* mft;
	std::map<size_t, Mft_Item*> mft_items;
    bool* bitmap;
	std::vector<Mft_Item*> current_path;
	int drive_id;

	bool Load_MFT();
    
};

class Exist_Item{
public:
	size_t parent_ID;
	size_t uid;
    bool exists;
    bool path_exists;
	kiv_os::NFile_Attributes is_directory;
};

class Functions{
public:
	static size_t Create_Item(VFS*, std::string, std::vector<Mft_Item*>, kiv_os::NFile_Attributes);
	static bool Move_To_Directory(VFS*, std::string, std::vector<Mft_Item*>&);
	static void Remove_Item(VFS*, std::string, std::vector<Mft_Item*> &);
    static Exist_Item* Check_Path(VFS*, std::string, std::vector<Mft_Item*>);
    static bool Is_Directory_Empty(VFS*, Exist_Item*);
    static void Move_To_Path(VFS*, std::string, std::vector<Mft_Item*>&);
    static bool Is_Bitmap_Writable(VFS*, size_t);
    static void Write_To_Data_Block(VFS*, Mft_Item*);
    static void Remove_From_Data_Block(VFS*, Mft_Item*);
    static Mft_Item* Get_Mft_Item(VFS*, size_t);
	static std::vector<Mft_Item*> Get_Items_In_Directory(VFS*, size_t);

	static void Save_VFS_MFT(VFS*);
	static void Save_VFS_MFT_Item(VFS*, size_t);

	static void Process_Sectors(kiv_hal::NDisk_IO operation, int drive_id, size_t count, size_t lba_index, void* sector);

	static void Print_Bitmap(VFS*);
	static void Print_MFT(VFS*);
    
};



#endif /* Header_h */
