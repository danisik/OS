#include "command_parser.h"

namespace command_parser {
	std::vector<Command> Find_Commands(char input[]) {
		std::vector<Command> commands = std::vector<Command>();
		std::stringstream stream(input);
		std::string token;

		bool end = false;
		bool first = true;
		while (true) {
			if (first) {
				if (!(stream >> token)) {
					break;
				}
			}

			if (end) {
				break;
			}

			first = false;

			Command command;
			command.base = token;
			command.parameters = "";
			if (!(stream >> token)) {
				commands.push_back(command);
				break;
			}
			while ((token != "cd" && token != "echo" && token != "tasklist" && token != "rd" && token != "md" && token != "type"
				&& token != "find" && token != "sort" && token != "dir" && token != "rgen" && token != "freq"
				&& token != "shell" && token != "shutdown" && token != "exit" && token != "test")) {
				command.parameters.append(token);
				if (!(stream >> token)) {
					end = true;
					break;
				}
				command.parameters.append(" ");
			}
			commands.push_back(command);
		}

		return commands;
	}
}