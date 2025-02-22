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
#include "../api/hal.h"
#include "../api/api.h"

#define ROOT_CHAR "~"
#define SPLIT " "
#define FOLDER_SPLIT "/"

const int32_t MFT_FRAGMENTS_COUNT = 32;
const int32_t UID_ITEM_FREE = 0;

class Mft_Item 
{
    
    public:
        Mft_Item(size_t, kiv_os::NFile_Attributes, std::string, size_t, size_t);
    
	    size_t uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
	    size_t parent_ID;
	    kiv_os::NFile_Attributes is_directory;                                   //soubor, nebo adresar
        int item_order;                                  //poradi v MFT pri vice souborech, jinak 1
        int item_order_total;                            //celkovy pocet polozek v MFT
        char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
	    size_t item_size;                                  //velikost souboru v bytech
	    std::vector<size_t> fragment_start_cluster;			//start adresa
        std::vector<size_t>  fragment_cluster_count;             //pocet clusteru ve fragmentu
        std::vector<int>  bitmap_start_ID;

};

class Boot_Record
{
    public:
        Boot_Record(uint64_t, uint16_t);
	    uint64_t Get_Cluster_Count() 
        { 
            return this->cluster_count;
        };
	    size_t disk_size;              //celkova velikost VFS			bytes_per_sector * absolute_number_of_sectors
        uint16_t cluster_size;           //velikost clusteru			TDrive_Parameters::bytes_per_sector
        uint64_t cluster_count;          //pocet clusteru				TDrive_Parameters::absolute_number_of_sectors
        size_t mft_start_cluster;      //cluster pocatku mft				
	    size_t bitmap_start_cluster;   //cluster pocatku bitmapy			
	    size_t data_start_cluster;     //cluster pocatku datovych bloku	
        long mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
};

class MFT
{
    
    public:
        MFT();
	    size_t UID_counter;
	    size_t size;
};

class VFS
{
    public:
        VFS(uint64_t, uint16_t, int);
	    std::unique_ptr<Boot_Record> boot_record;
        MFT* mft;
	    std::map<size_t, Mft_Item*> mft_items;
        std::vector<bool> bitmap;
    	int drive_id;

	    void Init_VFS(std::unique_ptr<VFS>&);
};

class Exist_Item
{
    public:
	    size_t parent_ID;
	    size_t uid;
        bool exists;
        bool path_exists;
	    kiv_os::NFile_Attributes is_directory;
};

class Functions
{
    public:
	    static size_t Create_Item(std::unique_ptr<VFS>&, std::string, std::vector<Mft_Item*>, kiv_os::NFile_Attributes);
	    static bool Move_To_Directory(std::unique_ptr<VFS>&, std::string, std::vector<Mft_Item*>&);
	    static void Remove_Item(std::unique_ptr<VFS>&, std::string, std::vector<Mft_Item*> &);
	    static void Update_Parent_ID(std::unique_ptr<VFS>&, size_t old_id, size_t new_id);
        static Exist_Item* Check_Path(std::unique_ptr<VFS>&, std::string, std::vector<Mft_Item*>);
        static bool Is_Directory_Empty(std::unique_ptr<VFS>&, Exist_Item*);
        static void Move_To_Path(std::unique_ptr<VFS>&, std::string, std::vector<Mft_Item*>&);
        static bool Is_Bitmap_Writable(std::unique_ptr<VFS>&, size_t);
        static void Write_To_Data_Block(std::unique_ptr<VFS>&, Mft_Item*);
        static void Remove_From_Data_Block(std::unique_ptr<VFS>&, Mft_Item*);
        static Mft_Item* Get_Mft_Item(std::unique_ptr<VFS>&, size_t);
	    static std::vector<Mft_Item*> Get_Items_In_Directory(std::unique_ptr<VFS>&, size_t);

	    static void Save_VFS_MFT(std::unique_ptr<VFS>&);
	    static void Save_VFS_MFT_Item(std::unique_ptr<VFS>&, size_t);

	    static void Process_Sectors(kiv_hal::NDisk_IO operation, int drive_id, size_t count, size_t lba_index, void* sector);
    
};



#endif /* Header_h */
