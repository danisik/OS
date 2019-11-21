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

	void Open_File(kiv_hal::TRegisters &regs); // Creating empty folders only.
	void Write_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Read_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Seek(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Close_Handle(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Delete_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Set_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
	void Get_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
	void Create_Pipe(kiv_hal::TRegisters &regs); // Not implemented yet.
};