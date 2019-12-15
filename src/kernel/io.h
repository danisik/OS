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

		void Open_File(kiv_hal::TRegisters &regs); // Tested.
		void Write_File(kiv_hal::TRegisters &regs); // Tested (missing pipe).
		void Read_File(kiv_hal::TRegisters &regs); // Tested (missing pipe).
		void Seek(kiv_hal::TRegisters &regs); // Tested.
		void Close_Handle(kiv_hal::TRegisters &regs); // Tested.
		void Delete_File(kiv_hal::TRegisters &regs); // Tested.
		void Set_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
		void Get_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
		void Create_Pipe(kiv_hal::TRegisters &regs); // Not implemented yet.
};