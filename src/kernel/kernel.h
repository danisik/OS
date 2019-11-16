#pragma once


#include "..\api\hal.h"
#include "..\api\api.h"

#include "io.h"
#include "io_process.h"

void Set_Error(const bool failed, kiv_hal::TRegisters &regs);
void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context);

kiv_os::THandle Shell_Clone();
void Shell_Wait(kiv_os::THandle handle);
void Shell_Close(kiv_os::THandle shell_handle, kiv_os::THandle std_in, kiv_os::THandle std_out);

