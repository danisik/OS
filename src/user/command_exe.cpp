#include "command_exe.h"
#include "rtl.h"

void Execute_Commands(std::vector<command_parser::Command> commands) {
	for each (command_parser::Command command in commands) {
		kiv_os::THandle handle;
		// TODO Execute_Commands: Create kiv_os::NClone clone_type
		kiv_os::NClone clone_type = kiv_os::NClone::Create_Process;

		kiv_os_rtl::Clone(clone_type, command.base.data(), command.parameters.data(), NULL, NULL, handle);
	}
}