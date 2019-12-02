#include "type.h"

size_t __stdcall type(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	size_t read = 1;
	size_t written = 0;
	const size_t buffer_size = 1024;
	size_t actual_position = 0;
	char buffer[buffer_size];
	kiv_os::THandle in_handle;
	std::string output = "";

	if (strlen(arguments) == 0) {
		char exit_message[40] = "The syntax of the command is incorrect\n";
		kiv_os_rtl::Write_File(std_out, exit_message, sizeof(exit_message), written);
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Invalid_Argument);
		kiv_os_rtl::Exit(exit_code);
		return exit_code;
	}
	else {
		bool open_result = kiv_os_rtl::Open_File(arguments, kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::System_File, in_handle);
		
		if (in_handle == static_cast<kiv_os::THandle>(-1)) {
			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::File_Not_Found);
			kiv_os_rtl::Exit(exit_code);
			return 0;
		}
	}

	// Set seek of file to file beginning.
	kiv_os_rtl::Seek(in_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, actual_position);

	while (read) {
		kiv_os_rtl::Read_File(in_handle, buffer, buffer_size, read);
		if (read > 0) output.append(buffer, read);
		actual_position += buffer_size;
		kiv_os_rtl::Seek(in_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, actual_position);
	}

	kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

	kiv_os_rtl::Close_Handle(in_handle);

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}