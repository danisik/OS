#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace command_parser {
	class Command {
		public:
			std::string base;
			std::string parameters;
		};

	std::vector<Command> Find_Commands(char input[]);
}