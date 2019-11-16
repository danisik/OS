#include "freq.h"

size_t __stdcall freq(const kiv_hal::TRegisters &regs) {
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}