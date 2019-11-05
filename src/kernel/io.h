#pragma once

#include "..\api\api.h"

void Handle_IO(kiv_hal::TRegisters &regs);

void Open_File(kiv_hal::TRegisters &regs);
void Write_File(kiv_hal::TRegisters &regs);
void Read_File(kiv_hal::TRegisters &regs);
void Seek(kiv_hal::TRegisters &regs);
void Close_Handle(kiv_hal::TRegisters &regs);
void Delete_File(kiv_hal::TRegisters &regs);
void Set_Working_Dir(kiv_hal::TRegisters &regs);
void Get_Working_Dir(kiv_hal::TRegisters &regs);
void Create_Pipe(kiv_hal::TRegisters &regs);