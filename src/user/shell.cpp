#include "shell.h"

#include "rtl.h"

const char* new_line = "\n";
const char* prompt = "C:\\>";
const char* welcome_message = "Welcome in OS";

size_t __stdcall shell(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const size_t buffer_size = 256;
	char buffer[buffer_size];
	size_t counter;

	//Print welcome message.
	kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
	kiv_os_rtl::Write_File(std_out, welcome_message, strlen(welcome_message), counter);
	kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);

	//While cycle for commands.
	while(1) {
		kiv_os_rtl::Write_File(std_out, prompt, strlen(prompt), counter);

		if (kiv_os_rtl::Read_File(std_in, buffer, buffer_size, counter) && (counter > 0)) {
			
			if (counter == buffer_size) {
				counter--;
			}

			buffer[counter] = 0;	//udelame z precteneho vstup null-terminated retezec

			if (strcmp(buffer, "exit") == 0) {
				break;
			}

			// Tohle tam ve v�sledku nebude.			
			kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
		}
		else {
			break;	//EOF
		}
	}

	return 0;
}