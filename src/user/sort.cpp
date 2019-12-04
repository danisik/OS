#include "sort.h"

size_t __stdcall sort(const kiv_hal::TRegisters &regs) {
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	char buffer[512];
	size_t read = 0;
	kiv_os_rtl::Read_File(std_in, buffer, 512, read);
	size_t write = 0;
	kiv_os_rtl::Write_File(std_out, buffer, read, write);

	kiv_os_rtl::Exit(exit_code);
	return 0;
}