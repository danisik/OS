//
//  IoUtils.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 22/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include "IoUtils.hpp"

bool IoUtils::is_file_exists(std::string filename)
{
    if (FILE *file = fopen(filename.c_str(), "r")) {
        fclose(file);
        return true;
    }
    
    return false;
}

FILE* IoUtils::create_new_ntfs_file(Vfs *vfs)
{
    FILE* file = std::fopen(vfs->filename.data(), "w+");
    
    return file;
}

bool IoUtils::write_vfs(Vfs *vfs)
{
    
    
    return true;
}

std::vector<std::string> IoUtils::split(const std::string str, const std::string delim)
{
    std::vector<std::string> tokens;
    tokens.clear();
    size_t prev = 0, pos = 0;
    
    do {
        pos = str.find(delim, prev);
        
        if (pos == std::string::npos) pos = str.length();
        
        std::string token = str.substr(prev, pos - prev);
        
        if (!token.empty()) tokens.push_back(token);
        
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    
    return tokens;
}

int32_t IoUtils::check_path(Vfs *vfs, std::string path, bool ignore_last)
{
    std::vector<std::string> split_path;
    split_path.clear();
    
    split_path = IoUtils::split(path, "/");
    
    int32_t tmp_uid = vfs->current_item->uid;
    Mft_item *tmp_item = vfs->current_item;
    
    if (split_path.size() == 0)
    {
        return tmp_uid;
    }
    
    //remove last element <- filename
    if (ignore_last)
    {
        split_path.pop_back();
    }
    
    
    
    for (auto it = split_path.begin(); it < split_path.end(); ++it)
    {
        if ((*it).compare("..") == 0 && tmp_item->parent_id == -1)
        {
            return -1;
        }
        
        if ((*it).compare("..") == 0)
        {
            tmp_item = vfs->find_mft_item_by_uid(tmp_item->parent_id);
            tmp_uid = tmp_item->uid;
            continue;
        }
        
        tmp_item = vfs->find_mft_item_by_name_and_parent((*it), tmp_item->uid);
        
        if (!tmp_item || tmp_item->parent_id != tmp_uid || !tmp_item->isDirectory)
        {
            return -1;
        }
        
        tmp_uid = tmp_item->uid;
    }
    
    return tmp_item->uid;
}

bool IoUtils::check_file(Vfs *vfs, std::string path, int32_t parent_id)
{
    std::string file = IoUtils::get_filename(vfs, path);
    
    auto item = vfs->find_mft_item_by_name_and_parent(file, parent_id);
    
    if (!item)
    {
        return false;
    }

    return true;
}

bool IoUtils::check_filename(Vfs *vfs, std::string filename, int32_t parent_id)
{
    std::string file = IoUtils::get_filename(vfs, filename);
    
    auto item = vfs->find_mft_item_by_name_and_parent(file, parent_id);
    
    if (!item)
    {
        return true;
    }
    
    return false;
}

std::string IoUtils::get_filename(Vfs* vfs, std::string filename)
{
    std::vector<std::string> split_path = IoUtils::split(filename, "/");
    
    if (split_path.size() == 0)
    {
        return vfs->current_item->item_name;
    }

    return split_path.back();
}
