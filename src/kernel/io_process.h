#pragma once

#include "../api/api.h"
#include "process.h"

#include <vector>

void Handle_Process(kiv_hal::TRegisters &regs);

void Clone(kiv_hal::TRegisters &regs);
void Clone_Process(kiv_hal::TRegisters &regs);
void Clone_Thread(kiv_hal::TRegisters &regs);

void Wait_For(kiv_hal::TRegisters &regs);
void Read_Exit_Code(kiv_hal::TRegisters &regs);
void Exit(kiv_hal::TRegisters &regs);
void Shutdown(kiv_hal::TRegisters &regs);
void Register_Signal_Handler(kiv_hal::TRegisters &regs);