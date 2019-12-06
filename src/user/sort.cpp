#include "sort.h"

bool sort_terminated = false;

size_t Sort_Terrminated_Checker(const kiv_hal::TRegisters &regs) {
	sort_terminated = true;
	return 0;
}

size_t __stdcall sort(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	kiv_os::NSignal_Id signal = kiv_os::NSignal_Id::Terminate;
	kiv_os::TThread_Proc handler = reinterpret_cast<kiv_os::TThread_Proc>(Sort_Terrminated_Checker);

	kiv_os_rtl::Register_Signal_Handler(signal, handler);

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
	size_t read = 1;
	char buffer[512];
	std::string complete = "";

	while (read && !sort_terminated) {
		kiv_os_rtl::Read_File(in_handle, buffer, sizeof(buffer), read);

		if (buffer[0] == kiv_hal::NControl_Codes::EOT) {
			break;
		}

		complete.append(buffer, 0, read);

		if (is_file) {
			actual_position += read;
			kiv_os_rtl::Seek(in_handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, actual_position);
		}
	}

	if (is_file) {
		kiv_os_rtl::Close_Handle(in_handle);
	}

	std::vector<std::string> lines;

	std::istringstream iss(complete);
	std::copy(std::istream_iterator<std::string>(iss),
		std::istream_iterator<std::string>(),
		std::back_inserter(lines));

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