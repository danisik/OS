#pragma once

#include "..\api\api.h"
#include "command_parser.h"
#include "command_exe.h"
#include "rtl.h"

#include <vector>

#define PATH_MAX 4096

extern "C" size_t __stdcall shell(const kiv_hal::TRegisters &regs);
