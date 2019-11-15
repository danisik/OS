#pragma once

#include "..\api\api.h"
#include "rtl.h"
#include "global.h"

#include <string>

#define ECHO_ON "on"
#define ECHO_OFF "off"
#define ECHO_HELP "/?"

extern "C" size_t __stdcall echo(const kiv_hal::TRegisters &regs);
