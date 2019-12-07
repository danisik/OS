#pragma once

#include "..\api\api.h"
#include "rtl.h"

extern "C" size_t __stdcall freq(const kiv_hal::TRegisters &regs);
void Assign_Frequencies(std::vector<int> &freq_array, char buffer[], size_t count);
void Init_Freq_Array(std::vector<int> &freq_array);
