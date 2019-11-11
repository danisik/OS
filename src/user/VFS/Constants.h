//
//  Constants.h
//  NTFS
//
//  Created by Jan Čarnogurský on 21/01/2019.
//  Copyright © 2019 Jan Čarnogurský. All rights reserved.
//

#ifndef Constants_h
#define Constants_h

#include <map>
#include <string>


const int32_t UID_ITEM_FREE = 0;

const int32_t MFT_FRAGMENTS_COUNT = 32;

const float MFT_PERCENTAGE_USAGE = 0.1; //we expect, that MFT usage 10% from space

const int32_t DISK_SIZE = 30720;
//const int32_t DISK_SIZE = 10000;

const int32_t CLUSTER_SIZE = 1024;


enum StringValue
{
    defaultVal,
    cp,
    mv,
    rm,
    mkdir,
    rmdir,
    ls,
    cat,
    cd,
    pwd,
    info,
    incp,
    outcp,
    load,
    format,
    defrag,
    touch,
    end
};

static std::map<std::string, StringValue> s_mapCommandValues;



static void initialize_commands()
{
    s_mapCommandValues["cp"] = cp;
    s_mapCommandValues["mv"] = mv;
    s_mapCommandValues["rm"] = rm;
    s_mapCommandValues["mkdir"] = mkdir;
    s_mapCommandValues["rmdir"] = rmdir;
    s_mapCommandValues["ls"] = ls;
    s_mapCommandValues["cat"] = cat;
    s_mapCommandValues["cd"] = cd;
    s_mapCommandValues["pwd"] = pwd;
    s_mapCommandValues["info"] = info;
    s_mapCommandValues["incp"] = incp;
    s_mapCommandValues["outcp"] = outcp;
    s_mapCommandValues["load"] = load;
    s_mapCommandValues["format"] = format;
    s_mapCommandValues["defrag"] = defrag;
    s_mapCommandValues["touch"] = touch;
    s_mapCommandValues["exit"] = end;
}

static void initialize_enum_types()
{
    initialize_commands();
}
#endif /* Constants_h */
