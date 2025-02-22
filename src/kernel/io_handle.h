#pragma once

#include "Header.h"
#include "io_process.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include "pipe.h"

#include <iomanip>


class IO_Handle 
{
	
	public:
		size_t seek = 1;
		virtual size_t Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
		virtual size_t Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
		virtual size_t Seek(kiv_os::NFile_Seek new_position, size_t position, size_t size);
		virtual void Close();
};

class STD_Handle_In : public IO_Handle 
{
	public:
		size_t Read(char *buffer, size_t buffer_lengthm, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
};

class STD_Handle_Out : public IO_Handle 
{
	public:
		size_t Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
};

class Pipe_Handle : public IO_Handle 
{
	public:
		Pipe *pipe;
		Pipe_Function function;
		Pipe_Handle(Pipe *p_pipe);
		Pipe_Handle(int p_buffer_size);
		size_t Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
		size_t Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
		void Close();
};

class Item_Handle : public IO_Handle 
{
	public:
		Mft_Item *item = nullptr;
};

class File_Handle : public Item_Handle 
{
	public:
		size_t Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
		size_t Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
};

class Directory_Handle : public Item_Handle 
{
	public:
		size_t Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
};

class Procfs_Handle : public Item_Handle 
{
	public:
		size_t Read(char *buffer, size_t buffer_lengthm, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process);
};
