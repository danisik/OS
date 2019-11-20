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
        Mft_Item* mftItem = new Mft_Item(0, true, "", 1, -1, false, -1);
        vfs->mft->UID_counter++;
        vfs->current_path.push_back(mftItem);
        Functions::Save_Vfs_To_File(vfs);
        if(Functions::Is_Bitmap_Writable(vfs, vfs->current_path[0]->item_size)){
            Functions::Write_To_Data_Block(vfs, vfs->current_path[0]);
        }
    }
    else{
        vfs = Functions::Load_VFS(file);
    }
    
    char buffer[100];
    string command;
    string tok;
    size_t i;
    bool isUsedFile = false;
    FILE* commandFile = nullptr;
    
    while(1) {
        
        VFS::Print_Current_Path(vfs);
        
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
                Commands::Create_Directory(vfs, command);
            }
        
        }
        else if(strcmp(tok.data(), MOVE_TO_DIRECTORY)==0){
            if(i==string::npos){
                Commands::Move_To_Root(vfs);
            }else{
                Commands::Move_To_Directory(vfs, command);
            }
            
            
        }
        else if(strcmp(tok.data(), LIST)==0){
            if(i== string::npos) {
                Commands::List(vfs);
            }else {
                Commands::List_With_Params(vfs, command);
            }
            
        }
        else if(strcmp(tok.data(), PWD)==0){
            Commands::Pwd(vfs);
        }
        else if(strcmp(tok.data(), REMOVE_DIR)==0){
            if(i==string::npos){
                cout << "FOLDER NOT SPECIFIED" << endl;
            }
            else {
                Commands::Remove_Directory(vfs, command);
            }
        }
        else if(strcmp(tok.data(), IMPORT_FILE)==0){
            i = command.find(SPLIT);
            string source =  command.substr(0, i);
            command = command.substr(i+1);
            i = command.find(SPLIT);
            string dest =  command.substr(0, i);
            Commands::Import_File(vfs, source, dest);
        }
        else if(strcmp(tok.data(), PRINT_FILE)==0){
            if(i==string::npos){
                cout << "FILE NOT SPECIFIED" << endl;
            }
            else {
                Commands::Print_File(vfs, command);
            }
        }
        else if(strcmp(tok.data(), MOVE_FILE)==0){
            i = command.find(SPLIT);
            string source =  command.substr(0, i);
            command = command.substr(i+1);
            i = command.find(SPLIT);
            string dest =  command.substr(0, i);
            Commands::Move_File(vfs, source, dest);
        }
        else if(strcmp(tok.data(), COPY_FILE)==0){
            i = command.find(SPLIT);
            string source =  command.substr(0, i);
            command = command.substr(i+1);
            i = command.find(SPLIT);
            string dest =  command.substr(0, i);
            Commands::Copy_File(vfs, source, dest);
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
                Commands::Info(vfs, command);
            }
        }
        else if(strcmp(tok.data(), SYSINFO)==0){
            Functions::Print_Bitmap(vfs);
            Functions::Print_MFT(vfs);
            Functions::Print_Boot_Record(vfs);
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
                Commands::Export_File(vfs, source, dest);
            }
        }
        else if(strcmp(tok.data(), FORMAT)==0){
            if(i==string::npos){
                cout << "SIZE NOT SPECIFIED" << endl;
            }
            else{
                
                vfs = Commands::Format(vfs, command);
                Mft_Item* mftItem = new Mft_Item(0, true, "root", 1, -1, false, -1);
                vfs->mft->UID_counter++;
                vfs->current_path.push_back(mftItem);
                Functions::Save_Vfs_To_File(vfs);
                if(Functions::Is_Bitmap_Writable(vfs, vfs->current_path[0]->item_size)){
                    Functions::Write_To_Data_Block(vfs, vfs->current_path[0]);
                }
                Functions::Save_Vfs_To_File(vfs);
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
                Commands::Create_Symlink(vfs, link, source);
            }
        }
        else if (strcmp(tok.data(), "exit")==0){
            Functions::Save_Vfs_To_File(vfs);
            break;
        }
        else {
            cout << "neznamy prikaz" << endl;
        }
        
        
    }
    
    printf("pseudoNTFS end\n");
    
    return 0;
}



