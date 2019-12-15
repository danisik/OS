#include "md.h"

size_t __stdcall md(const kiv_hal::TRegisters &regs) {
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	
	char *file_name = reinterpret_cast<char*>(regs.rdi.r);
	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(0);

	kiv_os::THandle handle;

	kiv_os_rtl::Open_File(file_name, flags, kiv_os::NFile_Attributes::Directory, handle);
	kiv_os_rtl::Close_Handle(handle);

	kiv_os_rtl::Exit(exit_code);
	return 0;
}