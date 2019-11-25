#include "dir.h"

size_t __stdcall dir(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	size_t written;
	size_t read;
	char buffer[1024];
	if (strlen(arguments) == 0) {
		strcpy_s(buffer, arguments);
	}
	else {
		kiv_os_rtl::Get_Working_Dir(buffer, sizeof(buffer), written);
	}

	size_t index = 0;
	int dir_count = 0;
	int file_count = 0;
	const int max_item_count = 10;
	char entries[sizeof(kiv_os::TDir_Entry) * max_item_count];
	kiv_os::THandle handle;
	std::string output = "";
	kiv_os_rtl::Open_File(buffer, kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::Read_Only, handle);
	kiv_os_rtl::Read_File(handle, entries, sizeof(entries), read);

	while (read) {
		while (index < max_item_count) {
			kiv_os::TDir_Entry *entry = reinterpret_cast<kiv_os::TDir_Entry *>(entries + index * sizeof(kiv_os::TDir_Entry));
			if (entry->file_attributes == static_cast<uint16_t>(kiv_os::NFile_Attributes::Directory)) {
				output.append("<DIR>");
				dir_count++;
			}
			else {
				output.append("<FILE>");
				file_count++;
			}
			
			output.append("\t");
			output.append(entry->file_name);
			output.append("\n");

			index++;
		}
		index = 0;

		kiv_os_rtl::Read_File(handle, entries, sizeof(entries), read);
	}

	std::ostringstream stream;

	output.append("\t\t");
	stream << file_count;
	output.append(stream.str());
	output.append("File(s)\n");

	stream.clear();

	output.append("\t\t");
	stream << dir_count;
	output.append(stream.str());
	output.append("Dir(s)\n");

	kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}