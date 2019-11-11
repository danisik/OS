//
//  Functions.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 25/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include "Commands.hpp"


void Commands::create_new_directory(Vfs *vfs, std::string name)
{
    int parent_id = IoUtils::check_path(vfs, name, true);
    if(parent_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
    
    if(!IoUtils::check_filename(vfs, name, parent_id))
    {
        printf("EXIST\n");
        return;
    }
    
    auto item = vfs->create_new_item(vfs->create_uniq_mft_uid(), parent_id, true, 0, 0, IoUtils::get_filename(vfs, name), 1);
    
    if (!item)
    {
        printf("ERROR\n");
    }
    else
    {
        vfs->write_vfs();
        printf("OK\n");
    }
}

void Commands::show_files(Vfs *vfs, std::string path)
{
    
    int item_id = IoUtils::check_path(vfs, path, false);

    if(item_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
 
    Mft_item *item = vfs->find_mft_item_by_uid(item_id);
    
    std::vector<Mft_item*>::iterator it;
    for(it = vfs->mft_items.begin() ; it != vfs->mft_items.end() ; ++it)
    {
        if ((*it)->parent_id == item->uid)
        {
            if ((*it)->isDirectory)
            {
                std::cout << "+ ";
            }
            else
            {
                std::cout << "- ";
            }
            
            std::cout << (*it)->item_name << std::endl;
        }
    }
}

void Commands::move_to_directory(Vfs *vfs, std::string path)
{
    int item_id = IoUtils::check_path(vfs, path, false);
    
    if(item_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
  
    std::vector<std::string> split_path = IoUtils::split(path, "/");
    
    for (auto it = split_path.begin() ; it < split_path.end() ; ++it)
    {
        if ((*it).compare("..") == 0)
        {
            vfs->current_path.pop_back();
        }
        else
        {
            vfs->current_path.push_back((*it));
        }
    }
    
    Mft_item *item = vfs->find_mft_item_by_uid(item_id);
    
    vfs->current_item = item;
    
    printf("OK\n");
    
}

void Commands::print_current_path(Vfs *vfs)
{
    std::cout << vfs->print_path() << std::endl;
}

void Commands::create_new_file(Vfs *vfs, std::string name)
{
    int parent_id = IoUtils::check_path(vfs, name, true);
    if(parent_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
    
    if(!IoUtils::check_filename(vfs, name, parent_id))
    {
        printf("EXIST\n");
        return;
    }
    
    auto item = vfs->create_new_item(vfs->create_uniq_mft_uid(), parent_id, false, 0, 0, IoUtils::get_filename(vfs, name), 1);
    
    if (!item)
    {
        printf("ERROR\n");
    }
    else
    {
        printf("OK\n");
    }
}

void Commands::copy_file(Vfs* vfs, std::string filename, std::string destination)
{
    int d_parent_id = IoUtils::check_path(vfs, destination, false);
    if(d_parent_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
    
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    
    Mft_item *s_item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    Mft_item *d_folder = vfs->find_mft_item_by_uid(d_parent_id);
    
    auto item = vfs->create_new_item(vfs->create_uniq_mft_uid(), d_folder->uid, s_item->isDirectory, 0, 0, s_item->item_name, s_item->item_size);
    
    if (!item)
    {
        printf("ERROR\n");
    }
    else
    {
        vfs->write_vfs();
        printf("OK\n");
    }
}

void Commands::move_file(Vfs* vfs, std::string filename, std::string destination)
{
    int d_parent_id = IoUtils::check_path(vfs, destination, false);
    if(d_parent_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
    
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    Mft_item *s_item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    Mft_item *d_folder = vfs->find_mft_item_by_uid(d_parent_id);
    
    s_item->parent_id = d_folder->uid;
    
    vfs->write_vfs();
    
    printf("OK\n");
}

void Commands::remove_file(Vfs* vfs, std::string filename)
{
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    Mft_item *item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    if (!item->isDirectory)
    {
        vfs->remove_mft_item(item);
    }
    else
    {
        printf("FILE NOT FOUND\n");
    }
    
    printf("OK\n");
}

void Commands::remove_directory(Vfs* vfs, std::string filename)
{
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    Mft_item *item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    if (!item->isDirectory)
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    
    if (vfs->get_children_count(item->uid) == 0)
    {
        vfs->remove_mft_item(item);
    }
    else
    {
        printf("NOT EMPTY\n");
        return;
    }
    
    printf("OK\n");
}

void Commands::print_item_details(Vfs* vfs, std::string filename)
{
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    Mft_item *item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    vfs->print_mft_item(item);
}

void Commands::insert_file_from_hdd(Vfs* vfs, std::string filename, std::string destination)
{
    if (!IoUtils::is_file_exists(filename))
    {
        printf("FILE NOT FOUND");
        return;
    }
    
    int s_parent_id = IoUtils::check_path(vfs, destination, false);
    if(s_parent_id < 0)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
    
    FILE *source = std::fopen(filename.c_str(), "rb");
    
    Mft_item *d_item = vfs->find_mft_item_by_uid(s_parent_id);
    
    bool result = vfs->insert_file(source, d_item, IoUtils::get_filename(vfs, filename));
    
    if (result)
    {
        printf("OK\n");
    }
}

void Commands::print_file_content(Vfs *vfs, std::string filename)
{
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    Mft_item *item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    vfs->print_content(item);
}

void Commands::export_file_from_ntfs(Vfs *vfs, std::string filename, std::string destination)
{
    int s_parent_id = IoUtils::check_path(vfs, filename, true);
    
    if(!IoUtils::check_file(vfs, filename, s_parent_id))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    Mft_item *item = vfs->find_mft_item_by_name_and_parent(IoUtils::get_filename(vfs, filename), s_parent_id);
    
    FILE *file = std::fopen((destination + item->item_name).c_str(), "wb");
    if (file == NULL)
    {
        printf("PATH NOT FOUND\n");
        return;
    }
    
    vfs->export_file(item, file);
    
    fclose(file);
    
    printf("OK\n");
}

void Commands::format_vfs(Vfs *vfs, std::string size, std::string type)
{
    
    
    vfs->mft_items.clear();
    
    int convert_number = 1024;
    
    if (type.compare("mb") == 0)
    {
        convert_number = convert_number * 1024;
    }
    else if (type.compare("gb") == 0)
    {
        convert_number = convert_number * 1024 * 1024;
    }
    
    int32_t newSize = atoi(size.c_str()) * convert_number;
    
    //valgrind is now crying
    vfs->boot_record = new BootRecord(newSize, CLUSTER_SIZE);
    vfs->bitmap = new Bitmap(vfs->boot_record->cluster_count);
    
    vfs->current_path.clear();
    vfs->create_root();
    
    vfs->write_vfs();
    
    printf("OK\n");
}

void Commands::load_commands(std::string path)
{
    if (!IoUtils::is_file_exists(path))
    {
        printf("FILE NOT FOUND\n");
        return;
    }
    
    std::ifstream file(path.c_str());
    std::string line;
    while (std::getline(file, line))
    {
         Ntfs::do_action(line);
    }
    
    printf("OK\n");
}

void Commands::defrag(Vfs *vfs)
{
    vfs->defrag_files();
    printf("OK\n");
}

void Commands::exit(Vfs *vfs)
{
    vfs->write_vfs();
    printf("OK\n");
}
