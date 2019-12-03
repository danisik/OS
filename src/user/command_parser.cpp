#include "command_parser.h"

namespace command_parser {
	std::vector<Command> Find_Commands(char input[]) {
		std::vector<Command> commands = std::vector<Command>();
		std::stringstream stream(input);
		std::string token;
		std::string file_name;

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
		
			while (true) {
				if(token == "|") {
					// pipe
					command.is_pipe = true;
					first = true;

					break;
				}

				if (token == ">") {
					// out
					command.file_name = "";
					while (stream >> token) {
						command.file_name.append(token);
					}
					end = true;
					command.is_red_out = true;
					
					break;
				}

				if (token == "<") {
					// in
					command.file_name = "";
					while (stream >> token) {
						command.file_name.append(token);
					}
					end = true;
					command.is_red_in = true;

					break;
				}
				// params
				command.parameters.append(token);
				if (!(stream >> token)) {
					end = true;

					break;
				}
				command.parameters.append(" ");
			}

			if (command.parameters.length() > 0 && command.parameters.at(command.parameters.length() - 1) == ' ') {
				command.parameters.pop_back();
			}

			commands.push_back(command);
		}

		return commands;
	}
}