#include "shell.h"

const char* new_line = "\n";
const char* prompt = "C:\\>";
const char* welcome_message = "Welcome in OS";

extern bool echo_on;
bool shutdown_signalized = false;

void Signalize_Shutdown() {
	shutdown_signalized = true;
}

size_t __stdcall shell(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	kiv_os_rtl::Register_Signal_Handler(kiv_os::NSignal_Id::Terminate, reinterpret_cast<kiv_os::TThread_Proc>(Signalize_Shutdown));

	const size_t buffer_size = 256;
	char buffer[buffer_size];
	size_t counter;

	// Print welcome message.
	kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
	kiv_os_rtl::Write_File(std_out, welcome_message, strlen(welcome_message), counter);
	kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);

	// While cycle for commands.
	while(1) {

		if (echo_on) {
			kiv_os_rtl::Write_File(std_out, prompt, strlen(prompt), counter);
		}

		if (kiv_os_rtl::Read_File(std_in, buffer, buffer_size, counter) && (counter > 0)) {
			
			if (counter == buffer_size) {
				counter--;
			}

			buffer[counter] = 0;

			if (strcmp(buffer, "exit") == 0) {
				break;
			}

			kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
			
			std::vector<command_parser::Command> commands = command_parser::Find_Commands(buffer);
			
			command_exe::Execute_Commands(commands, std_in, std_out);
		
			if (shutdown_signalized == true) {
				break;
			}

			kiv_os_rtl::Write_File(std_out, new_line, strlen(new_line), counter);
		}
		else {
			// TODO shell: Really ? Ask teacher.
			break;	// EOF.
		}
	}

	kiv_os_rtl::Exit(0);
	return 0;
}