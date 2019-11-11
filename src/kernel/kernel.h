#pragma once


#include "..\api\hal.h"
#include "..\api\api.h"
#include "user_programs.h"

#include "io.h"
#include "io_process.h"

void Set_Error(const bool failed, kiv_hal::TRegisters &regs);
void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context);
