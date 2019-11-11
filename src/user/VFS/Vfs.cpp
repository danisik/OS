//
//  vfs.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include "Vfs.hpp"


Vfs::Vfs(std::string filename, int32_t size)
{
    this->filename = filename;
    
    this->boot_record = new BootRecord(size, CLUSTER_SIZE);
    this->bitmap = new Bitmap(this->boot_record->cluster_count);
}

Vfs::Vfs(std::string filename)
{
    this->filename = filename;
}

int32_t Vfs::create_uniq_mft_uid()
{
    int32_t uid;
    do {
        uid = (rand() % 100 + 1); //0-100
        
    } while (this->is_uid_used(uid));
    
    return uid;
}

bool Vfs::is_uid_used(int32_t uid)
{
    std::vector<Mft_item*>::iterator it;
    for(it = mft_items.begin() ; it != mft_items.end() ; ++it)
    {
        if ((*it)->uid == uid)
        {
            return true;
        }
    }
    
    return false;
}

void Vfs::create_root()
{
    
    Mft_item* item = this->create_new_item(1, -1, true, 0, 0, "~", 1);
    this->current_item = item;
    
    this->current_path.push_back("~");
}

void Vfs::print_mft_items()
{
    std::cout << "==========================="<< std::endl;
    
    std::vector<Mft_item*>::iterator it;
    for(it = mft_items.begin() ; it != mft_items.end() ; ++it)
    {
        std::cout << "==========================="<< std::endl;
        this->print_mft_item((*it));
    }
}

void Vfs::print_mft_item(Mft_item *item)
{
    std::cout << "UID: " + std::to_string(item->uid) << std::endl;
    std::cout << "Name: " << (item->item_name) << std::endl;
    std::cout << "Parent UID: " + std::to_string(item->parent_id) << std::endl;
    std::cout << "Folder: " << (item->isDirectory ? "Yes" : "No") << std::endl;
    std::cout << "Size: " + std::to_string(item->item_size ) << " B" << std::endl;
    std::cout << "Fragments count: " + std::to_string(item->fragments_count ) << std::endl;
    std::cout << "Contains clusters: " + std::to_string(item->cluster_count ) << std::endl;
    
    std::cout << "Fragments: " << std::endl;
 
    int i = 1;
    std::vector<Mft_fragment*>::iterator it;
    for(it = item->fragments.begin() ; it != item->fragments.end() ; ++it)
    {
        std::cout << i <<" => Start address: " + std::to_string((*it)->fragment_start_address) << std::endl;
        std::cout << "     Clusters in row: " + std::to_string((*it)->fragment_count) << std::endl;
        i++;
    }
}

void Vfs::write_vfs()
{
    this->boot_record->mft_items_count = this->mft_items.size();
    
    this->boot_record->write(this->file);

    write_items();
    
    this->bitmap->write(file, this->boot_record->bitmap_start_address);
}

void Vfs::write_items()
{
    int i = 0;
    int fragments = 0;
    
    std::vector<Mft_item*>::iterator it;
    for(it = this->mft_items.begin() ; it != this->mft_items.end() ; ++it)
    {
        //fseek(this->file, this->boot_record->mft_start_address + (sizeof(Mft_item) * i), SEEK_SET);
        fseek(this->file, this->boot_record->mft_start_address + (sizeof(Mft_item) * i) + (sizeof(Mft_fragment) * fragments), SEEK_SET);
        fwrite((*it), sizeof(Mft_item), 1, this->file);
        fflush(this->file);
        
        std::vector<Mft_fragment*>::iterator it2;
        for(it2 = (*it)->fragments.begin() ; it2 != (*it)->fragments.end() ; ++it2)
        {
            fwrite((*it2), sizeof(Mft_fragment), 1, this->file);
            fflush(this->file);
            fragments++;
        }

        i++;
    }
}

void Vfs::read_exists_vfs()
{
    BootRecord *new_boot_record = new BootRecord();
    fread(new_boot_record, sizeof(BootRecord), 1, this->file);
    
    this->boot_record = new_boot_record;
    this->boot_record->print_bootrecord_details();
   
    this->read_items();
    
    
    Bitmap *new_bitmap = new Bitmap(this->boot_record->cluster_count);
    fseek(this->file, this->boot_record->bitmap_start_address, SEEK_SET);
    fread(new_bitmap->map, sizeof(bool), new_bitmap->size, this->file);
    
    this->bitmap = new_bitmap;
}

void Vfs::read_items()
{
    int fragments = 0;
    
    for (int i = 0; i < this->boot_record->mft_items_count; i++)
    {
        Mft_item *item = new Mft_item();
        fseek(this->file, this->boot_record->mft_start_address + (sizeof(Mft_item) * i) + (sizeof(Mft_fragment) * fragments), SEEK_SET);
        //fseek(this->file, this->boot_record->mft_start_address + (sizeof(Mft_item) * i), SEEK_SET);
        fread(item, sizeof(Mft_item), 1, this->file);
        this->mft_items.push_back(item);
        
        
        //set root, it's ugly, but it works 
        if (i == 0)
        {
            this->current_path.push_back(item->item_name);
            this->current_item = item;
        }
        
        std::vector<Mft_fragment*> tmp;
        
        for (int j = 0 ; j < item->fragments_count ; j++)
        {
            Mft_fragment *fragment = new Mft_fragment();
            fread(fragment, sizeof(Mft_fragment), 1, this->file);
            
         /*   //it's ugly, but it works, sometimes infinity loop
            if (fragment->fragment_start_address == 0)
            {
                break;
            }
          */
            tmp.push_back(fragment);
            
            fragments++;
        }
        item->fragments = tmp;
    }
}



Mft_item* Vfs::create_new_item(int32_t uid, int32_t parent_id, bool isDirectory, int8_t item_order, int8_t item_order_total, std::string name, int32_t size)
{
    if(size > this->get_left_disk_space())
    {
        printf("File is to large. Left disk space: %d B, file size: %d B\n", this->get_left_disk_space(), size);
        return NULL;
    }
    
    char *cstr = &name[0u];
    
    Mft_item *item = new Mft_item(uid, parent_id, isDirectory, item_order, item_order_total, cstr, size);
    
    this->set_up_fragments(&item, size);
    
    this->mft_items.push_back(item);
    
    return item;
}

void Vfs::set_up_fragments(Mft_item **item, int32_t size)
{
    (*item)->fragments_count = 0;
    
    //round to down
    int cluster_needed = size / CLUSTER_SIZE;
    
    //add cluster if devide is not zero
    if ((size % CLUSTER_SIZE != 0))
    {
        cluster_needed++;
    }
    
    (*item)->cluster_count = cluster_needed;
    
    int free_id = -1;
    int in_row = 1;
    int found = 0;
    int32_t start_address = this->boot_record->data_start_address + 1; //default data address
    int32_t cluster_address;
    
    do
    {
        free_id = this->bitmap->find_free_cluster(0);
        
        if (free_id == -1)
        {
            printf("No more free space\n");
            break;
        }
        
        found++;
        cluster_address = free_id * CLUSTER_SIZE;
        
        if (cluster_needed > 1)
        {
            for (int i = free_id + 1 ; i < this->bitmap->size ; i++)
            {
                if (this->bitmap->is_cluster_free(i))
                {
                    in_row++;
                    found++;
                }
                else
                {
                    Mft_fragment *fragment = new Mft_fragment(start_address + cluster_address, in_row, free_id);
                    (*item)->add_fragment(fragment);
                    
                    this->mark_bitmap(free_id, in_row);
                    
                    break;
                }
                
                if (found == cluster_needed)
                {
                    Mft_fragment *fragment = new Mft_fragment(start_address + cluster_address, in_row, free_id);
                    (*item)->add_fragment(fragment);
                    
                    this->mark_bitmap(free_id, in_row);
                    
                    break;
                }
            }
        }
        else
        {
            Mft_fragment *fragment = new Mft_fragment(start_address + cluster_address, in_row, free_id);
            (*item)->add_fragment(fragment);
            
            this->mark_bitmap(free_id, in_row);
            
            break;
        }
        
        in_row = 1;
        
    }while(found < cluster_needed);
    
    //just for check, this option should never happen
    if (found != cluster_needed)
    {
        std::cout << "BITMAP FULL" << "Found: " << found << " Needed: " << cluster_needed << std::endl;
    }
    
    this->write_vfs();
}

void Vfs::mark_bitmap(int from, int count)
{
    for (int i = from ; i < from + count ; i++)
    {
        this->bitmap->set_as_used(i);
    }
}

Mft_item* Vfs::find_mft_item_by_name(std::string name)
{
    std::vector<Mft_item*>::iterator it;
    for(it = this->mft_items.begin() ; it != this->mft_items.end() ; ++it)
    {
        if ((*it)->item_name == name)
        {
            return (*it);
        }
    }    
    return NULL;
}

Mft_item* Vfs::find_mft_item_by_uid(int32_t uid)
{
    std::vector<Mft_item*>::iterator it;
    for(it = this->mft_items.begin() ; it != this->mft_items.end() ; ++it)
    {
        if ((*it)->uid == uid)
        {
            return (*it);
        }
    }
    return NULL;
}

Mft_item* Vfs::find_mft_item_by_name_and_parent(std::string name, int32_t parent_id)
{
    std::vector<Mft_item*>::iterator it;
    for(it = this->mft_items.begin() ; it != this->mft_items.end() ; ++it)
    {
        std::string itemName = (*it)->item_name;
        if (itemName.compare(name) == 0 && (*it)->parent_id == parent_id)
        {
            return (*it);
        }
    }
    
    return NULL;
}

std::string Vfs::print_path()
{
    std::ostringstream vts;
    
    // Convert all but the last element to avoid a trailing "/"
    std::copy(this->current_path.begin(), this->current_path.end()-1, std::ostream_iterator<std::string>(vts, "/"));
    
    // Now add the last element with no delimiter
    vts << this->current_path.back();
    
    return vts.str();
}

void Vfs::remove_mft_item(Mft_item* mft_item)
{
    std::vector<Mft_fragment*>::iterator it;
    for(it = mft_item->fragments.begin() ; it != mft_item->fragments.end() ; ++it)
    {
        int to = (*it)->fragment_start_index + (*it)->fragment_count;
        for (int i = (*it)->fragment_start_index ; i < to ; i++)
        {
            this->bitmap->set_as_free(i);
        }
    }
    
    this->mft_items.erase(std::remove(this->mft_items.begin(), this->mft_items.end(), mft_item));
    
    this->write_vfs();
}

int Vfs::get_children_count(int parent_id)
{
    int count = 0;
    
    std::vector<Mft_item*>::iterator it;
    for(it = this->mft_items.begin() ; it != this->mft_items.end() ; ++it)
    {
        if ((*it)->parent_id == parent_id)
        {
            count++;
        }
    }
    
    return count;
}

bool Vfs::insert_file(FILE *source, Mft_item *destination, std::string filename)
{
    fseek(source, 0, SEEK_END);
    int32_t size = (int32_t)ftell(source);
    fseek(source, 0, SEEK_SET);
    
    
    if(size > this->get_left_disk_space())
    {
        printf("File is to large. Left disk space: %d B, file size: %d B\n", this->get_left_disk_space(), size);
        return false;
    }
    
    Mft_item *item = this->create_new_item(this->create_uniq_mft_uid(), destination->uid, false, 0, 0, filename, size);
    
    char buffer[item->cluster_count][CLUSTER_SIZE];
    
    for (int i = 0 ; i < item->cluster_count ; i++)
    {
        fseek(source, i * CLUSTER_SIZE, SEEK_SET);
        fread(buffer[i], CLUSTER_SIZE, 1, source);
    }
    
    
    int buffer_index = 0;
    int in_row_index = 0;
    
    std::vector<Mft_fragment*>::iterator it;
    for(it = item->fragments.begin() ; it != item->fragments.end() ; ++it)
    {
        fseek(this->file, (*it)->fragment_start_address, SEEK_SET);
        do
        {
            fwrite(buffer[buffer_index], CLUSTER_SIZE, 1, this->file);
            
            fflush(this->file);
            
            
            buffer_index++;
            in_row_index++;
        }while(in_row_index < (*it)->fragment_count);
        
        in_row_index = 0;
        
    }
    
    this->write_vfs();
    
    return true;
}

void Vfs::print_content(Mft_item *item)
{
    char buffer[CLUSTER_SIZE];
    
    int in_row_index = 0;
    
    std::vector<Mft_fragment*>::iterator it;
    for(it = item->fragments.begin() ; it != item->fragments.end() ; ++it)
    {
        fseek(this->file, (*it)->fragment_start_address, SEEK_SET);
        
        do
        {
            fread(buffer, CLUSTER_SIZE, 1, this->file);
            printf("%s", buffer);
            
            in_row_index++;
        }while(in_row_index < (*it)->fragment_count);
        
        in_row_index = 0;
    }
    
    printf("\n");
}

void Vfs::export_file(Mft_item *source, FILE *output)
{
    char buffer[CLUSTER_SIZE];
    
    int in_row_index = 0;
    
    std::vector<Mft_fragment*>::iterator it;
    for(it = source->fragments.begin() ; it != source->fragments.end() ; ++it)
    {
        fseek(this->file, (*it)->fragment_start_address, SEEK_SET);
        
        do
        {
            fread(buffer, CLUSTER_SIZE, 1, this->file);
            fwrite(buffer, strlen(buffer), 1, output);
            fflush(output);
            
            in_row_index++;
        }while(in_row_index < (*it)->fragment_count);
        
        in_row_index = 0;
    }
    
    fputc(0x0a, output);
}

void Vfs::defrag_files()
{
    std::map<int32_t, std::vector<std::string>> temp;
    
    char buffer[CLUSTER_SIZE];
    int in_row_index = 0;
    
    //iterate over all items
    std::vector<Mft_item*>::iterator item_iterator;
    for(item_iterator = this->mft_items.begin() ; item_iterator != this->mft_items.end() ; ++item_iterator)
    {
        std::vector<std::string> temp_fragments;
        
        //iterate over all fragments in item
        std::vector<Mft_fragment*>::iterator fragment_iterator;
        for(fragment_iterator = (*item_iterator)->fragments.begin() ; fragment_iterator != (*item_iterator)->fragments.end() ; ++fragment_iterator)
        {
            //set fragment start address
            fseek(this->file, (*fragment_iterator)->fragment_start_address, SEEK_SET);
            
            do
            {
                //read data from fragment
                fread(buffer, CLUSTER_SIZE, 1, this->file);
                
                //save data to vector
                temp_fragments.push_back(std::string(buffer));
                
                in_row_index++;
            }while(in_row_index < (*fragment_iterator)->fragment_count);
            
            //insert data vector to map
            temp.insert(std::make_pair((*item_iterator)->uid, temp_fragments));
            
            in_row_index = 0;
        }
        
        //clear item fragments
        (*item_iterator)->fragments.clear();
    }
    
    //reset bitmap
    this->bitmap->reset_bitmap();
    
    //write data to new clusters
    //iterate over map
    for(auto map_iterator = temp.begin() ; map_iterator != temp.end() ; ++map_iterator)
    {
        //find item
        auto item = this->find_mft_item_by_uid(map_iterator->first);
        
        
        //generate new fragments
        this->set_up_fragments(&item, item->item_size);
        
        in_row_index = 0;
        
        //fill fragments with data
        std::vector<Mft_fragment*>::iterator it;
        for(it = item->fragments.begin() ; it != item->fragments.end() ; ++it)
        {
            fseek(this->file, (*it)->fragment_start_address, SEEK_SET);
            
            do
            {
                //write data to new cluster
                fwrite(temp[item->uid].front().c_str(), CLUSTER_SIZE, 1, this->file);
                fflush(this->file);
                
                //remove written data
                temp[item->uid].erase(temp[item->uid].begin());
                
                in_row_index++;
            }while(in_row_index < (*it)->fragment_count);
            
            in_row_index = 0;
        }
    }
    
    this->write_vfs();
    
}

void Vfs::set_current_item(Mft_item *item)
{
    this->current_item = item;
}

int32_t Vfs::get_left_disk_space()
{
    return (DISK_SIZE - ((this->bitmap->free_cluster_count() - this->bitmap->size) * CLUSTER_SIZE));
}

