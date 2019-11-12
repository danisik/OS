#pragma once

#include "..\api\api.h"

#define ECHO_ON "on"
#define ECHO_OFF "off"
#define ECHO_HELP "/?"

extern "C" size_t __stdcall echo(const kiv_hal::TRegisters &regs);
