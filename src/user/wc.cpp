#include "wc.h"
#include "rtl.h"

#include <vector>
#include <string>
#include <sstream>

size_t __stdcall wc(const kiv_hal::TRegisters &regs) {
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

	if (strcmp(part1, "/v") == 0 && strcmp(part2, "/c\"\"") == 0 && other.size() == 0) {
		size_t read;
		char buffer[512];
		std::string complete = "";
		bool res = kiv_os_rtl::Read_File(std_in, buffer, sizeof(buffer), read);
		if (!res) {
			//TODO error
			return 0;
		}
		complete.append(buffer);
		while (read) {
			res = kiv_os_rtl::Read_File(std_in, buffer, sizeof(buffer), read);
			if (!res) {
				//TODO error
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

		std::string output = std::to_string(lines);
		output.append("\n");
		size_t written;
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	}
	else {
		//TODO error
	}

	return 0;	
}