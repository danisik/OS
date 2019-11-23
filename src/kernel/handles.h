#pragma once

#include "..\api\api.h"

#include <Windows.h>
#include <mutex>
#include "../kernel/VFS/header.h"

kiv_os::THandle Convert_Native_Handle(const HANDLE hnd);
HANDLE Resolve_kiv_os_Handle(const kiv_os::THandle hnd);
bool Remove_Handle(const kiv_os::THandle hnd);

class IO_Handle {
	private:
		std::mutex handle_mutex;
	public:
		virtual size_t Read(char *buffer, size_t buffer_length, VFS *vfs);
		virtual size_t Write(char *buffer, size_t buffer_length, VFS *vfs);
		virtual void Close();
};

class STD_Handle_In : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_lengthm, VFS *vfs);
};

class STD_Handle_Out : public IO_Handle {
public:
	size_t Write(char *buffer, size_t buffer_length, VFS *vfs);
};

class File_Handle : public IO_Handle {
	public:
		size_t file_id;
		size_t seek;

		size_t Read(char *buffer, size_t buffer_length, VFS *vfs);
		size_t Write(char *buffer, size_t buffer_length, VFS *vfs);
};

class Directory_Handle : public IO_Handle {
	public:
		size_t directory_id;

		size_t Read(char *buffer, size_t buffer_length, VFS *vfs);
};

class Pipe_Handle : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length, VFS *vfs);
		size_t Write(char *buffer, size_t buffer_length, VFS *vfs);
		void Close();
};
