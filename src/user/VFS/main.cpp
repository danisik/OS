//
//  main.cpp
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#include <iostream>
#include "Ntfs.hpp"


int main(int argc, const char * argv[]) {
    
    Ntfs *ntfs = new Ntfs("/Users/cagy/Documents/skola/ZOS/NTFS/ntfs.dat");
    ntfs->create_vfs();

    return 0;
}
