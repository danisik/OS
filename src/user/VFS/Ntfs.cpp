//
//  ntfs.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include "Ntfs.hpp"

Vfs* Ntfs::vfs;

Ntfs::Ntfs(std::string filename)
{
    this->filename = filename;
}

void Ntfs::create_vfs()
{
    //temp
    //remove(this->filename.c_str());
    
    
    
    if (IoUtils::is_file_exists(this->filename))
    {
        Ntfs::vfs = new Vfs(this->filename);
        Ntfs::vfs->file = fopen(this->filename.c_str(), "r+");
        
        
        printf("Loading existing file...\n");
        Ntfs::vfs->read_exists_vfs();
        printf("Loaded\n");
    }
    else
    {
        Ntfs::vfs = new Vfs(this->filename, DISK_SIZE);
        FILE *file = fopen(this->filename.c_str(), "wb");
        fclose(file);
        
        Ntfs::vfs->file = fopen(this->filename.c_str(), "r+");
        Ntfs::vfs->create_root();
        printf("Creating new file...\n");
        
        Ntfs::vfs->write_vfs();
        std::cout <<  "Created" << std::endl;
    }
    
    initialize_enum_types();
    
    bool running = true;
    while(running)
    {
        
        std::cout << Ntfs::vfs->print_path() << " $ ";
        
        std::string input;
        std::getline (std::cin,input);
        
        if (input.empty())
        {
            continue;
        }
        else
        {
            running = Ntfs::do_action(input);
        }
        
        input.clear();
        std::cin.clear();
    
    }
    Ntfs::vfs->print_mft_items();
    
    fclose(Ntfs::vfs->file);
}

bool Ntfs::do_action(std::string command)
{
    std::vector<std::string>parameters;
    parameters.clear();
    
    parameters = IoUtils::split(command, " ");
    
    switch (s_mapCommandValues[parameters[0]]) {
        case cp:
            Commands::copy_file(Ntfs::vfs, parameters[1], parameters[2]);
            break;
        case mv:
            Commands::move_file(Ntfs::vfs, parameters[1], parameters[2]);
            break;
        case rm:
            Commands::remove_file(Ntfs::vfs, parameters[1]);
            break;
        case mkdir:
            Commands::create_new_directory(Ntfs::vfs, parameters[1]);
            break;
        case rmdir:
            Commands::remove_directory(Ntfs::vfs, parameters[1]);
            break;
        case ls:
            //std::cout << "\"" << parameters[1] << "\"" << std::endl;
            Commands::show_files(Ntfs::vfs, parameters[1]);
            break;
        case cat:
            Commands::print_file_content(Ntfs::vfs, parameters[1]);
            break;
        case cd:
            Commands::move_to_directory(Ntfs::vfs, parameters[1]);
            break;
        case pwd:
            Commands::print_current_path(Ntfs::vfs);
            break;
        case info:
            Commands::print_item_details(Ntfs::vfs, parameters[1]);
            break;
        case incp:
            Commands::insert_file_from_hdd(Ntfs::vfs, parameters[1], parameters[2]);
            break;
        case outcp:
            Commands::export_file_from_ntfs(Ntfs::vfs, parameters[1], parameters[2]);
            break;
        case load:
            Commands::load_commands(parameters[1]);
            break;
        case format:
            Commands::format_vfs(Ntfs::vfs, parameters[1], parameters[2]);
            break;
        case touch:
            Commands::create_new_file(Ntfs::vfs, parameters[1]);
            break;
        case defrag:
            Commands::defrag(Ntfs::vfs);
            break;
        case end:
            std::cout << "Closing..." << ".\n";
            Commands::exit(Ntfs::vfs);
            return false;
            break;
            
        default:
            std::cout << "Unknown command \"" << command << "\"\n";;
    }
    
    return true;
}



