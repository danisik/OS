#ifndef Constants_hpp
#define Constants_hpp

#include <stdio.h>

#define ROOT_CHAR "~"
#define SPLIT " "
#define FOLDER_SPLIT "/"



#define CREATE_DIRECTORY "mkdir"
#define MOVE_TO_DIRECTORY "cd"
#define LIST "ls"
#define PWD "pwd"
#define REMOVE_DIR "rmdir"
#define IMPORT_FILE "incp"
#define PRINT_FILE "cat"
#define MOVE_FILE "mv"
#define COPY_FILE "cp"
#define REMOVE_FILE "rm"
#define INFO "info"
#define CLEAR "clear"
#define LOAD "load"
#define FORMAT "format"
#define EXPORT "outcp"
#define SYMLINK "slink"
#define SYSINFO "sysinfo"

const int32_t MFT_FRAGMENTS_COUNT = 32;
const int32_t UID_ITEM_FREE = 0;
const int32_t CLUSTER_SIZE = 4000;

#endif /* Constants_hpp */
