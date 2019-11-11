//
//  IoUtils.hpp
//  NTFS
//
//  Created by Jan Čarnogurský on 22/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef IoUtils_hpp
#define IoUtils_hpp

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Vfs.hpp"
#include "BootRecord.hpp"
#include "Mft_item.h"


class IoUtils
{
public:
    static bool is_file_exists(std::string filename);
    static FILE* create_new_ntfs_file(Vfs *vfs);
    static bool write_vfs(Vfs *vfs);
    static bool write_cluster(Vfs *vfs);
    static bool write_bitmap(Vfs *vfs);
    static std::vector<std::string> split(std::string str, std::string delim);
    static int32_t check_path(Vfs* vfs, std::string path, bool ignore_last);
    static bool check_file(Vfs* vfs, std::string path, int32_t parent_id);
    static bool check_filename(Vfs* vfs, std::string filename, int32_t parent_id);
    static std::string get_filename(Vfs* vfs, std::string filename);
};
#endif /* IoUtils_hpp */
