#include "sort.h"

size_t __stdcall sort(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	std::string output;
	size_t written;

	std::string str = std::string(arguments);

	size_t actual_position = 0;
	kiv_os::THandle in_handle = std_in;
	bool is_file = false;
	if (str.length() >= 1) {
		kiv_os_rtl::Open_File(str.data(), kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::System_File, in_handle);
		is_file = true;
		kiv_os_rtl::Seek(in_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, actual_position);
	}

	if (in_handle == static_cast<kiv_os::THandle>(-1)) {
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::File_Not_Found);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}
	
	const int buffer_size = 512;
	size_t read = 1;
	std::vector<char> buffer(buffer_size);
	std::string complete = "";

	while (read) {
		kiv_os_rtl::Read_File(in_handle, buffer.data(), sizeof(buffer), read);
		if (buffer[0] == 4) {
			break;
		}
		complete.append(buffer.data(), 0, read);

		if (is_file) {
			actual_position += read;
			kiv_os_rtl::Seek(in_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, actual_position);
		}
	}

	if (is_file) {
		kiv_os_rtl::Close_Handle(in_handle);
	}

	std::vector<std::string> lines;
	std::string line;

	std::stringstream ss(complete);

	while (std::getline(ss, line)) {
		lines.push_back(line);
	}

	std::sort(lines.begin(), lines.end());

	complete.clear();

	size_t count = 0;
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		complete.append(*it);
		if (count < lines.size() - 1) {
			complete.append("\n");
		}
		count++;
	}

	output.append(complete);
	output.append("\n");
	kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	
	int16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}