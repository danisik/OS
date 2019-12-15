#pragma once
#include "command_parser.h"
#include "rtl.h"

#include <map>

namespace command_exe {
	void Execute_Commands(std::vector<command_parser::Command> &commands, kiv_os::THandle in, kiv_os::THandle out);
}