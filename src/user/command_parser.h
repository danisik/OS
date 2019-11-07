#pragma once
#include <vector>
#include <string>


namespace command_parser {
	class Command {
	public:
		std::string base;
		std::vector<std::string> parameters;
	};

	std::vector<Command> find_commands(std::vector<std::string> tokens);
	std::vector<std::string> parse_input(char input[]);
	std::vector<Command> get_commands(char input[]);
}