#include "type.h"
#include "rtl.h"

#include <string>

size_t __stdcall type(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	bool is_file;
	size_t read = 1;
	size_t written;
	char buffer[1024];
	kiv_os::THandle in_handle;
	std::string output = "";

	if (strlen(arguments) == 0) {
		in_handle = std_out;
		is_file = false;
	}
	else {
		bool open_result = kiv_os_rtl::Open_File(arguments, kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::Read_Only, in_handle);
		is_file = true;
		if (!open_result) {
			output = "File not found";
			uint16_t exit_code = static_cast<uint16_t>(static_cast<kiv_os::NOS_Error>(kiv_os_rtl::Last_Error));
			kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
			kiv_os_rtl::Exit(exit_code);
			return 0;
		}
	}

	while (read) {
		kiv_os_rtl::Read_File(in_handle, buffer, sizeof(buffer), read);
		output.append(buffer);
	}

	kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	
	if (is_file) {
		kiv_os_rtl::Close_Handle(in_handle);
	}

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}