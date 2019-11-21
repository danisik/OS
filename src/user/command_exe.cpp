#include "command_exe.h"

#include <string>

void cd(const char *new_directory, kiv_os::THandle out) {
	bool setted = true;
	kiv_os_rtl::Set_Working_Dir(new_directory, setted);
	if (!setted) {
		char *error_message = "The system cannot find the listed path.";

		size_t written = 0;
		kiv_os_rtl::Write_File(out, error_message, strlen(error_message), written);
	}
}

void command_exe::Execute_Commands(std::vector<command_parser::Command> commands, kiv_os::THandle in, kiv_os::THandle out) {
	kiv_os::THandle *handles = new kiv_os::THandle[commands.size()];
	int handles_count = 0;

	for each (command_parser::Command command in commands) {
		if (command.base != "cd" && command.base != "echo" && command.base != "ps" && command.base != "rd"
			&& command.base != "md" && command.base != "type" && command.base != "find" && command.base != "sort"
			&& command.base != "dir" && command.base != "rgen" && command.base != "freq" && command.base != "shell"
			&& command.base != "shutdown" && command.base != "exit") {

			std::string output = "Unknown command.\n";
			size_t written;
			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Unknown_Error);
			kiv_os_rtl::Write_File(out, output.data(), output.size(), written);
			return;
		}
		else {
			if (command.base == "cd") {
				cd(command.parameters.data(), out);
				return;
			}
			else {
				kiv_os::THandle handle;

				// Create process for new command.
				kiv_os_rtl::Clone_Process(command.base.data(), command.parameters.data(), in, out, handle);

				handles[handles_count] = handle;
				handles_count++;
			}
		}
	}

	// Wait for commands to be executed.
	kiv_os::THandle signalized_handler;

	kiv_os_rtl::Wait_For(handles, handles_count, signalized_handler);

	uint16_t exit_code = -1;
	kiv_os_rtl::Read_Exit_Code(signalized_handler, exit_code);
	
	delete handles;
}