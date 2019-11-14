#include "command_parser.h"

namespace command_parser {
	std::vector<Command> Find_Commands(std::vector<std::string> tokens) {
		std::vector<Command> commands = std::vector<Command>();
		size_t size = tokens.size();
		int i = 0;
		std::string token;
		while (i < size) {
			token = tokens[i];
			Command command;
			command.base = token;
			command.parameters = "";
			i++;
			if (i < size) {
				token = tokens[i];
			}
			while ((token != "cd" && token != "echo" && token != "ps" && token != "rd" && token != "md" && token != "type"
				&& token != "wc" && token != "sort" && token != "dir" && token != "rgen" && token != "freq"
				&& token != "shell" && token != "shutdown" && token != "exit") && i < size) {
				command.parameters.append(" ");
				command.parameters.append(token);
				i++;
				if (i < size) {
					token = tokens[i];
				}
			}
			commands.push_back(command);
		}

		return commands;
	}

	std::vector<std::string> Parse_Input(char input[]) {
		std::vector<std::string> tokens = std::vector<std::string>();

		char *next_token = NULL;
		char *token = strtok_s(input, " ", &next_token);

		while (token != NULL) {
			tokens.push_back(token);
			token = strtok_s(NULL, " ", &next_token);
		}

		return tokens;
	}

	std::vector<Command> Get_Commands(char input[]) {
		std::vector<std::string> command_line = Parse_Input(input);
		std::vector<Command> commands = Find_Commands(command_line);

		return commands;
	}
}