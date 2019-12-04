#pragma once

#include "../kernel/VFS/header.h"
#include "io_process.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include "pipe.h"


class IO_Handle {
	
	public:
		size_t seek = 1;
		virtual size_t Read(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
		virtual size_t Write(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
		virtual size_t Seek(kiv_os::NFile_Seek new_position, size_t position, size_t size);
		virtual void Close();
};

class STD_Handle_In : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_lengthm, VFS *vfs, IO_Process *io_process);
};

class STD_Handle_Out : public IO_Handle {
	public:
		size_t Write(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
};

class Pipe_Handle : public IO_Handle {
	public:
		Pipe *pipe;
		Pipe_Function function;
		Pipe_Handle(Pipe *p_pipe);
		Pipe_Handle(int p_buffer_size);
		size_t Read(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
		size_t Write(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
		void Close();
};

class Item_Handle : public IO_Handle {
	public:
		Mft_Item *item;
};

class File_Handle : public Item_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
		size_t Write(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
};

class Directory_Handle : public Item_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length, VFS *vfs, IO_Process *io_process);
};

class Procfs_Handle : public Item_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_lengthm, VFS *vfs, IO_Process *io_process);
};
