#include "dir.h"

size_t __stdcall dir(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	size_t written;
	size_t read;

	const int buffer_size = 512;
	std::vector<char> buffer(buffer_size);

	memcpy(buffer.data(), arguments, sizeof(arguments));
	
	size_t current_index = 0;
	size_t index = 0;
	int dir_count = 0;
	int file_count = 0;
	const int max_item_count = 20;
	
	char entries[sizeof(kiv_os::TDir_Entry) * max_item_count];
	kiv_os::THandle handle;
	std::string output = "";
	kiv_os_rtl::Open_File(buffer.data(), kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::Directory, handle);
	if (handle == static_cast<kiv_os::THandle>(-1)) {
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::File_Not_Found);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}
	// Set seek of directory to 0.
	kiv_os_rtl::Seek(handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, index);

	kiv_os_rtl::Read_File(handle, entries, sizeof(entries), read);

	while (read) {
		while (current_index < (read / sizeof(kiv_os::TDir_Entry))) {
			kiv_os::TDir_Entry *entry = reinterpret_cast<kiv_os::TDir_Entry *>(entries + current_index * sizeof(kiv_os::TDir_Entry));
			if (entry->file_attributes == static_cast<uint16_t>(kiv_os::NFile_Attributes::Directory)) {
				output.append("<DIR>");
				dir_count++;
			}
			else {
				file_count++;
			}
			
			output.append("\t\t");
			output.append(entry->file_name);
			output.append("\n");
			current_index++;
		}
		index += current_index;
		current_index = 0;

		// Set seek of directory to index which has not been yet processed.
		kiv_os_rtl::Seek(handle, kiv_os::NFile_Seek::Set_Position, kiv_os::NFile_Seek::Beginning, index);

		kiv_os_rtl::Read_File(handle, entries, sizeof(entries), read);
	}

	output.append("\n");
	output.append("File(s): ");
	output.append(std::to_string(file_count));
	output.append("\n");

	output.append("Dir(s): ");
	output.append(std::to_string(dir_count));
	output.append("\n");

	kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	kiv_os_rtl::Close_Handle(handle);

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}