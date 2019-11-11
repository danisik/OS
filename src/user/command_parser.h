#pragma once
#include <vector>
#include <string>

namespace command_parser {
	class Command {
		public:
			std::string base;
			std::string parameters;
		};

	std::vector<Command> Find_Commands(std::vector<std::string> tokens);
	std::vector<std::string> Parse_Input(char input[]);
	std::vector<Command> Get_Commands(char input[]);
}