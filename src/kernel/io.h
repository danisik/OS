#pragma once

#include "..\api\api.h"

#include "handles.h"
#include "io_process.h"
#include "VFS/header.h"

#include <mutex>

class IO {
private:
	IO_Process* io_process;
public:
	VFS *vfs;

	IO(IO_Process *i_io_process, VFS *i_vfs);
	void Handle_IO(kiv_hal::TRegisters &regs);

	void Print_VFS();

	void Open_File(kiv_hal::TRegisters &regs); // Tested (only creating in VFS, not in virtual drive).
	void Write_File(kiv_hal::TRegisters &regs); // Tested only for STD (missing file and pipe).
	void Read_File(kiv_hal::TRegisters &regs); // Tested only for STD (missing file and pipe).
	void Seek(kiv_hal::TRegisters &regs); // Tested.
	void Close_Handle(kiv_hal::TRegisters &regs); // Tested.
	void Delete_File(kiv_hal::TRegisters &regs); // Tested (only deleting in VFS, not in virtual drive).
	void Set_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
	void Get_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
	void Create_Pipe(kiv_hal::TRegisters &regs); // Not implemented yet.
};