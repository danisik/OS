#include "command_parser.h"

namespace command_parser {
	std::vector<Command> Find_Commands(char input[]) {
		std::vector<Command> commands = std::vector<Command>();
		std::stringstream stream(input);
		std::string token;
		while (true) {
			if (!(stream >> token)) {
				break;
			}
			Command command;
			command.base = token;
			command.parameters = "";
			if (!(stream >> token)) {
				commands.push_back(command);
				break;
			}
			while ((token != "cd" && token != "echo" && token != "ps" && token != "rd" && token != "md" && token != "type"
				&& token != "find" && token != "sort" && token != "dir" && token != "rgen" && token != "freq"
				&& token != "shell" && token != "shutdown" && token != "exit")) {
				command.parameters.append(token);
				if (!(stream >> token)) {
					break;
				}
				command.parameters.append(" ");
			}
			commands.push_back(command);
		}

		return commands;
	}
}