#pragma once

#include "..\api\api.h"

#include <Windows.h>
#include <mutex>

kiv_os::THandle Convert_Native_Handle(const HANDLE hnd);
HANDLE Resolve_kiv_os_Handle(const kiv_os::THandle hnd);
bool Remove_Handle(const kiv_os::THandle hnd);

class IO_Handle {
	private:
		std::mutex handle_mutex;
	public:
		virtual size_t Read(char *buffer, size_t buffer_length);
		virtual size_t Write(char *buffer, size_t buffer_length);
		virtual void Close();
};

class STD_Handle_In : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length);
};

class STD_Handle_Out : public IO_Handle {
public:
	size_t Write(char *buffer, size_t buffer_length);
};

class File_Handle : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length);
		size_t Write(char *buffer, size_t buffer_length);
};

class Directory_Handle : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length);
		size_t Write(char *buffer, size_t buffer_length);
};

class Pipe_Handle : public IO_Handle {
	public:
		size_t Read(char *buffer, size_t buffer_length);
		size_t Write(char *buffer, size_t buffer_length);
		void Close();
};
