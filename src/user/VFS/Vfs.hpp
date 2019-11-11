//
//  vfs.hpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Vfs_hpp
#define Vfs_hpp

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <map>

#include "BootRecord.hpp"
#include "Mft_item.h"
#include "Bitmap.hpp"



class Vfs
{
public:
    
    std::string filename;
    Vfs(std::string filename, int32_t size);
    Vfs(std::string filename);
    BootRecord *boot_record;
    std::vector<Mft_item*> mft_items;
    Bitmap *bitmap;
    FILE *file;
    
    BootRecord test;
    
    std::vector<std::string> current_path;
    
    Mft_item *current_item;
    
    int32_t create_uniq_mft_uid();
    bool is_uid_used(int32_t uid);
    void write_vfs();
    void write_items();
    void read_exists_vfs();
    void read_items();
    
    void create_root();
    void print_mft_items();
    void print_mft_item(Mft_item* item);
    void print_vfs_details();
    
    Mft_item *create_new_item(int32_t uid, int32_t parent_id, bool isDirectory, int8_t item_order, int8_t item_order_total, std::string name, int32_t size);
    void write_mft_item(Mft_item*);
    void set_up_fragments(Mft_item **item, int32_t size);
    
    void mark_bitmap(int from, int count);
    
    Mft_item *find_mft_item_by_name(std::string name);
    Mft_item *find_mft_item_by_uid(int32_t uid);
    Mft_item *find_mft_item_by_name_and_parent(std::string name, int32_t parent_id);
    
    void remove_mft_item(Mft_item* mft_item);
    int get_children_count(int parent_id);
    
    bool insert_file(FILE *source, Mft_item *destination, std::string filename);
    void print_content(Mft_item *item);
    void export_file(Mft_item *source, FILE *output);
    
    void defrag_files();
    void clear_bitmap();
    
    void set_current_item(Mft_item *item);
    
    std::string print_path();
    
    int32_t get_left_disk_space();
};


#endif /* vfs_hpp */
