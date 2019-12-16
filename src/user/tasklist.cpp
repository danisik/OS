#include "tasklist.h"

size_t __stdcall tasklist(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);

	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(0);
	kiv_os::NFile_Attributes attributes = static_cast<kiv_os::NFile_Attributes>(0);

	const size_t buffer_size = 512;
	std::vector<char> buffer(buffer_size);
	std::string output = "";
	size_t actual_position = 0;
	size_t read = 1;

	kiv_os::THandle handle;
	kiv_os_rtl::Open_File("procfs", flags, attributes, handle);
	kiv_os_rtl::Read_File(handle, buffer.data(), buffer_size, read);

	size_t writed = 0;
	kiv_os_rtl::Write_File(std_out, buffer.data(), read, writed);
	kiv_os_rtl::Write_File(std_out, "\n", 1, writed);

	kiv_os_rtl::Close_Handle(handle);

	kiv_os_rtl::Exit(exit_code);
	return 0;
}