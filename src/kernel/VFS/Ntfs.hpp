//
//  ntfs.hpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Ntfs_hpp
#define Ntfs_hpp

#include <stdio.h>

#include "Vfs.hpp"
#include "IoUtils.hpp"
#include "Constants.h"
#include "Commands.hpp"


class Ntfs
{
public:
    std::string filename;
    static Vfs* vfs;
    
    Ntfs(std::string filename);
    void create_vfs();
    static bool do_action(std::string command);
private:
};

#endif /* ntfs_hpp */
