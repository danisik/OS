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

	int freq_array[256];

	Init_Freq_Array(freq_array);
	
	char buffer[1];
	size_t read;

	kiv_os_rtl::Read_File(std_in, buffer, 1, read);

	Assign_Frequencies(freq_array, buffer, read);

	while (read) {
		kiv_os_rtl::Read_File(std_in, buffer, 1, read);
		Assign_Frequencies(freq_array, buffer, read);
	}

	output_string = "\n";
	kiv_os_rtl::Write_File(std_out, output_string.data(), output_string.size(), written);

	char output_buffer[256];
	int size;
	for (int i = 0; i < 256; i++) {
		if (freq_array[i] > 0) {
			size = sprintf_s(output_buffer, "0x%hhx : %d\n", (unsigned char) i, freq_array[i]);
			kiv_os_rtl::Write_File(std_out, output_buffer, size, written);
		}
	}
	

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}

void Assign_Frequencies(int *freq_array, char buffer[], size_t count) {
	unsigned int index;
	for (int i = 0; i < count; i++) {
		index = (unsigned char)buffer[i];
		unsigned char r = (unsigned char)buffer[i];
		freq_array[index]++;
	}
}

void Init_Freq_Array(int *freq_array) {
	for (int i = 0; i < 256; i++) {
		freq_array[i] = 0;
	}
}