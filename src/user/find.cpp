#include "find.h"

size_t __stdcall find(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	std::string output;
	size_t written;

	if (strlen(arguments) == 0) {
		output = "Wrong arguments.\n";
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Invalid_Argument);
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}

	std::stringstream stream(arguments);

	std::string part1;
	std::string part2;
	std::string rest;
	std::string other = "";

	stream >> part1;
	stream >> part2;

	while (stream >> rest) {
		other.append(rest);
	}

	if (part1 == "/v" && part2 == "/c\"\"") {
		kiv_os::THandle in_handle = std_in;
		if (rest.length() >= 1) {
			bool open_result = kiv_os_rtl::Open_File(arguments, kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::System_File, in_handle);
		}

		if (in_handle == static_cast<kiv_os::THandle>(-1)) {
			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::File_Not_Found);
			kiv_os_rtl::Exit(exit_code);
			return 0;
		}
		size_t read;
		char buffer[512];
		std::string complete = "";
		kiv_os_rtl::Read_File(in_handle, buffer, sizeof(buffer), read);
		complete.append(buffer);
		
		while (read) {
			kiv_os_rtl::Read_File(in_handle, buffer, sizeof(buffer), read);
			complete.append(buffer);
		}
		
		std::stringstream data(complete);
		std::string line;
		int lines = 0;
		while (std::getline(data, line)) {
			lines = lines + 1;
		}

		output = std::to_string(lines);
		output.append("\n");
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	}
	else {
		output = "Wrong arguments.\n";
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Invalid_Argument);
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}	
	int16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}