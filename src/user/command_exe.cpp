#include "command_exe.h"

void command_exe::Execute_Commands(std::vector<command_parser::Command> commands, kiv_os::THandle in, kiv_os::THandle out) {
	for each (command_parser::Command command in commands) {
		kiv_os::THandle handle;

		kiv_os_rtl::Clone_Process(command.base.data(), command.parameters.data(), in, out, handle);
	}
}