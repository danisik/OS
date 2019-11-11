#include "command_exe.h"
#include "rtl.h"

void Execute_Commands(std::vector<command_parser::Command> commands) {
	for each (command_parser::Command command in commands) {
		kiv_os::THandle handle;
		kiv_os::NOS_Process process_type;

		// TODO Execute_Commands: Create kiv_os::NOS_Process process_type
		kiv_os_rtl::Clone(process_type, command.base.data(), command.parameters.data(), NULL, NULL, handle);
	}
}