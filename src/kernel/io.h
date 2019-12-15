#pragma once

#include "..\api\api.h"

#include "handles.h"
#include "io_handle.h"
#include "io_process.h"
#include "Header.h"

#include <mutex>

#define PIPE_SIZE 4096

class IO 
{
	public:
		std::unique_ptr<VFS> vfs;
		std::unique_ptr<IO_Process> io_process;
		IO(uint64_t cluster_count, uint16_t cluster_size, int v_drive_i);

		void Handle_IO(kiv_hal::TRegisters &regs);

		void Print_To_Console(const char* message);

		void Open_File(kiv_hal::TRegisters &regs);
		void Write_File(kiv_hal::TRegisters &regs);
		void Read_File(kiv_hal::TRegisters &regs);
		void Seek(kiv_hal::TRegisters &regs);
		void Close_Handle(kiv_hal::TRegisters &regs);
		void Delete_File(kiv_hal::TRegisters &regs);
		void Set_Working_Dir(kiv_hal::TRegisters &regs);
		void Get_Working_Dir(kiv_hal::TRegisters &regs);
		void Create_Pipe(kiv_hal::TRegisters &regs);
};