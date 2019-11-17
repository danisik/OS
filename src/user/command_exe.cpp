#include "command_exe.h"

void command_exe::Execute_Commands(std::vector<command_parser::Command> commands, kiv_os::THandle in, kiv_os::THandle out) {
	kiv_os::THandle *handles = new kiv_os::THandle[commands.size()];
	int handles_count = 0;

	for each (command_parser::Command command in commands) {
		kiv_os::THandle handle;

		// Create process for new command.
		kiv_os_rtl::Clone_Process(command.base.data(), command.parameters.data(), in, out, handle);

		handles[handles_count] = handle;
		handles_count++;
	}

	// Wait for commands to be executed.
	kiv_os::THandle signalized_handler;
	kiv_os_rtl::Wait_For(handles, handles_count, signalized_handler);

	uint16_t exit_code = -1;
	kiv_os_rtl::Read_Exit_Code(signalized_handler, exit_code);
	
	delete handles;
}