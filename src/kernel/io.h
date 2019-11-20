#pragma once

#include "..\api\api.h"

#include "handles.h"
#include "io_process.h"

#include <mutex>

class IO {
private:
	IO_Process* io_process;
public:
	IO(IO_Process *i_io_process);
	void Handle_IO(kiv_hal::TRegisters &regs);

	void Open_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Write_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Read_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Seek(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Close_Handle(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Delete_File(kiv_hal::TRegisters &regs); // Not implemented yet.
	void Set_Working_Dir(kiv_hal::TRegisters &regs); // 1 / 2 implemented (need to check if directory exists).
	void Get_Working_Dir(kiv_hal::TRegisters &regs); // Tested.
	void Create_Pipe(kiv_hal::TRegisters &regs); // Not implemented yet.
};