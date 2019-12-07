#include "freq.h"


size_t __stdcall freq(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	std::string output_string;
	size_t written;

	if (strlen(arguments) != 0) {
		output_string = "Wrong arguments.\n";
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Invalid_Argument);
		kiv_os_rtl::Write_File(std_out, output_string.data(), output_string.size(), written);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}

	const int freq_buffer_size = 256;

	std::vector<int> freq_array(freq_buffer_size);

	Init_Freq_Array(freq_array);
	
	const int buffer_size = 1;
	std::vector<char> buffer(buffer_size);
	size_t read = 1;

	while (read) {
		kiv_os_rtl::Read_File(std_in, buffer.data(), 1, read);

		if (buffer[0] == kiv_hal::NControl_Codes::EOT) {
			break;
		}
		Assign_Frequencies(freq_array, buffer.data(), read);
	}

	output_string = "\n";
	kiv_os_rtl::Write_File(std_out, output_string.data(), output_string.size(), written);

	char output_buffer[freq_buffer_size];
	int size;
	for (int i = 0; i < freq_buffer_size; i++) {
		if (freq_array.data()[i] > 0) {
			size = sprintf_s(output_buffer, "0x%hhx : %d\n", (unsigned char) i, freq_array.data()[i]);
			kiv_os_rtl::Write_File(std_out, output_buffer, size, written);
		}
	}
	

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}

void Assign_Frequencies(std::vector<int> &freq_array, char buffer[], size_t count) {
	unsigned int index;
	for (int i = 0; i < count; i++) {
		index = (unsigned char)buffer[i];
		unsigned char r = (unsigned char)buffer[i];
		freq_array[index]++;
	}
}

void Init_Freq_Array(std::vector<int> &freq_array) {
	for (int i = 0; i < freq_array.size(); i++) {
		freq_array[i] = 0;
	}
}