#include "command_parser.h"

namespace command_parser {
	std::vector<Command> find_commands(std::vector<std::string> tokens) {
		std::vector<Command> commands = std::vector<Command>();
		size_t size = tokens.size();
		int i = 0;
		while (i < size) {
			std::string token = tokens[i];
			Command command;
			command.base = token;
			command.parameters = std::vector<std::string>();
			i++;
			if (i < size) {
				token = tokens[i];
			}
			while ((token != "cd" || token != "echo" || token != "ps" || token != "rd" || token != "md" || token != "type"
				|| token != "wc" || token != "sort" || token != "dir" || token != "rgen" || token != "freq"
				|| token != "shell" || token != "shutdown" || token != "exit") && i < size) {
				command.parameters.push_back(token);
				i++;
			}
			commands.push_back(command);
		}

		return commands;
	}

	std::vector<std::string> parse_input(char input[]) {
		std::vector<std::string> tokens = std::vector<std::string>();

		char *next_token = NULL;
		char *token = strtok_s(input, " ", &next_token);

		while (token != NULL) {
			tokens.push_back(token);
			token = strtok_s(NULL, " ", &next_token);
		}

		return tokens;
	}

	std::vector<Command> get_commands(char input[]) {
		std::vector<std::string> command_line = parse_input(input);
		std::vector<Command> commands = find_commands(command_line);

		return commands;
	}
}