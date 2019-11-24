#include "sort.h"

size_t __stdcall sort(const kiv_hal::TRegisters &regs) {
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);

	const char *file_name = reinterpret_cast<const char*>(regs.rdi.r);

	// Temporary test for opening file.
	kiv_os::THandle handle;
	char buffer[500];
	size_t read = 0;
	//kiv_os_rtl::Open_File(file_name, kiv_os::NOpen_File::fmOpen_Always, (kiv_os::NFile_Attributes)0, handle);
	kiv_os_rtl::Open_File(file_name, kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::Directory, handle);
	kiv_os_rtl::Read_File(handle, buffer, sizeof(buffer), read);

	kiv_os_rtl::Exit(exit_code);
	return 0;
}