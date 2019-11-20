#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Header.h"

using namespace std;


int main(int argc, const char * argv[]) {
    VFS *vfs;
    
	FILE *file;
	fopen_s(&file, "ntfs.dat", "r+");
    if(file==NULL){
        fopen_s(&file, "ntfs.dat", "w+");
        vfs = new VFS(file, 150000);
        MftItem* mftItem = new MftItem(0, true, "root", 1, -1, false, -1);
        vfs->mft->UIDcounter++;
        vfs->currentPath.push_back(mftItem);
        Functions::saveVfsToFile(vfs);
        if(Functions::isBitmapWritable(vfs, vfs->currentPath[0]->item_size)){
            Functions::writeToDataBlock(vfs, vfs->currentPath[0]);
        }
    }
    else{
        vfs = Functions::loadVFS(file);
    }
    
    char buffer[100];
    string command;
    string tok;
    size_t i;
    bool isUsedFile = false;
    FILE* commandFile = nullptr;
    
    while(1) {
        
        VFS::printCurrentPath(vfs);
        
        if(isUsedFile == false){
            command = "";
            getline(cin, command);
            
        }else{
            fgets(buffer, 100, commandFile);
            command = buffer;
            i = command.find("\n");
            command = command.substr(0, i);
            cout << command << endl;
            if(feof(commandFile)==1){
                fclose(commandFile);
                isUsedFile = false;
            }
        }
        
        i = command.find(SPLIT);
        tok = command.substr(0, i);
        command = command.substr(i+1);
        if(strcmp(tok.data(), CREATE_DIRECTORY)==0){
            if(i == string::npos){
                cout << "FOLDER NOT SPECIFIED" << endl;
            }
            else{
                Commands::createDirectory(vfs, command);
            }
        
        }
        else if(strcmp(tok.data(), MOVE_TO_DIRECTORY)==0){
            if(i==string::npos){
                Commands::moveToRoot(vfs);
            }else{
                Commands::moveToDirectory(vfs, command);
            }
            
            
        }
        else if(strcmp(tok.data(), LIST)==0){
            if(i== string::npos) {
                Commands::list(vfs);
            }else {
                Commands::listWithParams(vfs, command);
            }
            
        }
        else if(strcmp(tok.data(), PWD)==0){
            Commands::pwd(vfs);
        }
        else if(strcmp(tok.data(), REMOVE_DIR)==0){
            if(i==string::npos){
                cout << "FOLDER NOT SPECIFIED" << endl;
            }
            else {
                Commands::removeDirectory(vfs, command);
            }
        }
        else if(strcmp(tok.data(), IMPORT_FILE)==0){
            i = command.find(SPLIT);
            string source =  command.substr(0, i);
            command = command.substr(i+1);
            i = command.find(SPLIT);
            string dest =  command.substr(0, i);
            Commands::importFile(vfs, source, dest);
        }
        else if(strcmp(tok.data(), PRINT_FILE)==0){
            if(i==string::npos){
                cout << "FILE NOT SPECIFIED" << endl;
            }
            else {
                Commands::printFile(vfs, command);
            }
        }
        else if(strcmp(tok.data(), MOVE_FILE)==0){
            i = command.find(SPLIT);
            string source =  command.substr(0, i);
            command = command.substr(i+1);
            i = command.find(SPLIT);
            string dest =  command.substr(0, i);
            Commands::moveFile(vfs, source, dest);
        }
        else if(strcmp(tok.data(), COPY_FILE)==0){
            i = command.find(SPLIT);
            string source =  command.substr(0, i);
            command = command.substr(i+1);
            i = command.find(SPLIT);
            string dest =  command.substr(0, i);
            Commands::copyFile(vfs, source, dest);
        }
        else if(strcmp(tok.data(), REMOVE_FILE)==0){
            if(i==string::npos){
                cout << "FILE NOT SPECIFIED" << endl;
            }
            else {
                Commands::removeFile(vfs, command);
            }
        }
        else if(strcmp(tok.data(), INFO)==0){
            if(i==string::npos){
                cout << "FILE NOT SPECIFIED" << endl;
            }
            else {
                Commands::info(vfs, command);
            }
        }
        else if(strcmp(tok.data(), SYSINFO)==0){
            Functions::printBitmap(vfs);
            Functions::printMFT(vfs);
            Functions::printBootRecord(vfs);
        }
        else if(strcmp(tok.data(), LOAD)==0){
            fopen_s(&commandFile, command.c_str(), "r");
            if(commandFile==NULL){
                cout << "FILE NOT FOUND" <<endl;
            }
            else{
                isUsedFile = true;
            }
            
        }
        else if(strcmp(tok.data(), EXPORT)==0){
            if(i==string::npos){
                cout << "FILE NOT SPECIFIED"<<endl;
            }
            else {
                i = command.find(SPLIT);
                string source =  command.substr(0, i);
                command = command.substr(i+1);
                i = command.find(SPLIT);
                string dest =  command.substr(0, i);
                Commands::exportFile(vfs, source, dest);
            }
        }
        else if(strcmp(tok.data(), FORMAT)==0){
            if(i==string::npos){
                cout << "SIZE NOT SPECIFIED" << endl;
            }
            else{
                
                vfs = Commands::format(vfs, command);
                MftItem* mftItem = new MftItem(0, true, "root", 1, -1, false, -1);
                vfs->mft->UIDcounter++;
                vfs->currentPath.push_back(mftItem);
                Functions::saveVfsToFile(vfs);
                if(Functions::isBitmapWritable(vfs, vfs->currentPath[0]->item_size)){
                    Functions::writeToDataBlock(vfs, vfs->currentPath[0]);
                }
                Functions::saveVfsToFile(vfs);
            }
        }
        else if(strcmp(tok.data(), SYMLINK)==0){
            if(i==string::npos){
                cout << "WRONG PARAMETERS" << endl;
            }
            else {
                i = command.find(SPLIT);
                string link =  command.substr(0, i);
                command = command.substr(i+1);
                i = command.find(SPLIT);
                string source =  command.substr(0, i);
                Commands::createSymlink(vfs, link, source);
            }
        }
        else if (strcmp(tok.data(), "exit")==0){
            Functions::saveVfsToFile(vfs);
            break;
        }
        else {
            cout << "neznamy prikaz" << endl;
        }
        
        
    }
    
    printf("pseudoNTFS end\n");
    
    return 0;
}



