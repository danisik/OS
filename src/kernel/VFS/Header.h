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

//#include "../../api/api.h"

//using namespace std;

class Mft_Fragment {
public:
    Mft_Fragment(int, size_t, size_t);
    size_t fragment_start_cluster;			//start adresa
    size_t fragment_cluster_count;             //pocet clusteru ve fragmentu
    int bitmap_start_ID;
};


class Mft_Item {
    
public:
    Mft_Item(size_t, kiv_os::NFile_Attributes, std::string, long, size_t, bool, int);
    
	size_t uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
	size_t parent_ID;
	kiv_os::NFile_Attributes is_directory;                                   //soubor, nebo adresar
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
    bool Is_In_MFT(std::string);
	std::vector<Mft_Item*> mft_items;
	size_t UID_counter;
    int32_t size;
    
};

class VFS{
public:
    VFS(uint64_t, uint16_t);
    FILE* file;
    Boot_Record* boot_record;
    MFT* mft;
    bool* bitmap;
	std::vector<Mft_Item*> current_path;

	void Load_MFT();
	void Load_Bitmap();
	void Load_VFS();
    
};

class Commands{
public:
    static size_t Create_Item(VFS*, std::string, std::vector<Mft_Item*>, kiv_os::NFile_Attributes);
    static bool Move_To_Directory(VFS*, std::string, std::vector<Mft_Item*>&);
    static void Move_To_Root(VFS*);
    static void List_With_Params(VFS*, std::string);
    static void List(VFS*);
    static void Remove_Directory(VFS*, std::string, std::vector<Mft_Item*> &);
    static void removeFile(VFS*, std::string);
	static void Print_MFT(VFS*);
    static void Print_File(VFS*, std::string);
    static void Move_File(VFS*, std::string, std::string);
    static void Copy_File(VFS*, std::string, std::string);
    static void Info(VFS*, std::string);
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
    static Exist_Item* Check_Path(VFS*, std::string, std::vector<Mft_Item*>);
    static bool Is_Directory_Empty(VFS*, Exist_Item*);
    static void Move_To_Path(VFS*, std::string, std::vector<Mft_Item*>&);
    static bool Is_Bitmap_Writable(VFS*, long);
    static void Write_To_Data_Block(VFS*, Mft_Item*);
    static void Remove_From_Data_Block(VFS*, Mft_Item*);
    static void Write_To_Clusters(VFS*, Mft_Item*, FILE*);
    static void Copy_To_Clusters(VFS*,Mft_Item*, Mft_Item*);
    static void Save_Vfs_To_File(VFS*);
	static void Print_Bitmap(VFS*);
    static Mft_Item* Get_Mft_Item(VFS*, size_t);
    static void Print_Clusters(VFS*, Mft_Item*);
	static void Print_MFT(VFS* );
    static VFS* Load_VFS(FILE*);
    static void Delete_Links(VFS*, Mft_Item*);
	static std::vector<size_t> Get_Items_In_Directory(VFS*, size_t);
    
};



#endif /* Header_h */
