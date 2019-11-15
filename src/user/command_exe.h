#pragma once
#include "command_parser.h"
#include "rtl.h"

namespace command_exe {
	void Execute_Commands(std::vector<command_parser::Command> commands);
}