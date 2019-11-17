#include "command_exe.h"

void command_exe::Execute_Commands(std::vector<command_parser::Command> commands, kiv_os::THandle in, kiv_os::THandle out) {
	for each (command_parser::Command command in commands) {
		kiv_os::THandle handle;

		kiv_os_rtl::Clone_Process(command.base.data(), command.parameters.data(), in, out, handle);


		/*
		// Wait_For + Read_Exit_Code usage.
		kiv_os::THandle handles[1];
		handles[0] = handle;

		kiv_os::THandle signalized_handler;
		kiv_os_rtl::Wait_For(handles, signalized_handler);

		uint16_t exit_code = -1;
		kiv_os_rtl::Read_Exit_Code(signalized_handler, exit_code);
		printf("%d", exit_code);
		*/
	}
}