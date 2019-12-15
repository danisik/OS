#pragma once
#include <vector>
#include <string>
#include <sstream>

namespace command_parser {
	class Command {
		public:
			std::string base;
			std::string parameters;
			std::string file_name;
			bool is_pipe = false;
			bool is_red_out = false;
			bool is_red_in = false;
		};

	std::vector<Command> Find_Commands(char *input);
}