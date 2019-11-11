//
//  Commands
//  NTFS
//
//  Created by Jan Čarnogurský on 25/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Functions_hpp
#define Functions_hpp

#include <stdio.h>
#include "Vfs.hpp"
#include "IoUtils.hpp"
#include "Constants.h"
#include "Ntfs.hpp"
#include <fstream>
#include <string>

class Commands
{
public:
    static void create_new_directory(Vfs* vfs, std::string name);
    static void show_files(Vfs* vfs, std::string path);
    //static void move_files(Vfs* vfs)
    static void move_to_directory(Vfs* vfs, std::string where);
    static void print_current_path(Vfs* vfs);
    static void create_new_file(Vfs* vfs, std::string name);
    static void copy_file(Vfs* vfs, std::string filename, std::string destination);
    static void move_file(Vfs* vfs, std::string filename, std::string destination);
    static void remove_file(Vfs* vfs, std::string filename);
    static void remove_directory(Vfs* vfs, std::string filename);
    static void print_item_details(Vfs* vfs, std::string filename);
    static void insert_file_from_hdd(Vfs* vfs, std::string filename, std::string destination);
    static void export_file_from_ntfs(Vfs* vfs, std::string filename, std::string destination);
    static void print_file_content(Vfs *vfs, std::string filename);
    static void format_vfs(Vfs* vfs, std::string size, std::string type);
    static void load_commands(std::string path);
    static void defrag(Vfs* vfs);
    static void exit(Vfs* vfs);
};


#endif /* Commands_hpp */
