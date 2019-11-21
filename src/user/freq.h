#pragma once

#include "..\api\api.h"
#include "rtl.h"

extern "C" size_t __stdcall freq(const kiv_hal::TRegisters &regs);
void Assign_Frequencies(int *freq_array, char buffer[], size_t count);
void Init_Freq_Array(int *freq_array);
