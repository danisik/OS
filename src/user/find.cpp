#include "find.h"

size_t __stdcall find(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);
	std::vector<char> copy(arguments, arguments + strlen(arguments) + 1);

	std::string other = "";

	char *next_token = NULL;
	char *part1 = strtok_s(copy.data(), " ", &next_token);

	char *part2 = strtok_s(NULL, " ", &next_token);

	char *token = strtok_s(NULL, " ", &next_token);
	while (token != NULL) {
		other.append(token);
		token = strtok_s(NULL, " ", &next_token);
	}

	std::string output;
	size_t written;
	if (strcmp(part1, "/v") == 0 && strcmp(part2, "/c\"\"") == 0 && other.size() == 0) {
		size_t read;
		char buffer[512];
		std::string complete = "";
		bool res = kiv_os_rtl::Read_File(std_in, buffer, sizeof(buffer), read);
		if (!res) {
			output = "Read error.\n";
			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::IO_Error);
			kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
			kiv_os_rtl::Exit(exit_code);
			return 0;
		}
		complete.append(buffer);
		while (read) {
			res = kiv_os_rtl::Read_File(std_in, buffer, sizeof(buffer), read);
			if (!res) {
				output = "Read error.\n";
				uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::IO_Error);
				kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
				kiv_os_rtl::Exit(exit_code);
				return 0;
			}
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
	printf("ecc");
	int16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}