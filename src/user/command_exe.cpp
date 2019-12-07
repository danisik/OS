#include "command_exe.h"

#include <string>

void cd(const char *new_directory, kiv_os::THandle out) {
	bool setted = true;

	kiv_os_rtl::Set_Working_Dir(new_directory, setted);
	if (!setted) {
		char *error_message = "The system cannot find the listed path.\n";

		size_t written = 0;
		kiv_os_rtl::Write_File(out, error_message, strlen(error_message), written);
		printf("%s\n", new_directory);
	}
}

void command_exe::Execute_Commands(std::vector<command_parser::Command> commands, kiv_os::THandle in, kiv_os::THandle out) {
	std::vector<kiv_os::THandle> handless;
	std::map<size_t, kiv_os::THandle> pipes_in;
	std::map<size_t, kiv_os::THandle> pipes_out;
	size_t current_command_position = 0;

	for each (command_parser::Command command in commands) {
		if (command.base != "cd" && command.base != "echo" && command.base != "tasklist" && command.base != "rd"
			&& command.base != "md" && command.base != "type" && command.base != "find" && command.base != "sort"
			&& command.base != "dir" && command.base != "rgen" && command.base != "freq" && command.base != "shell"
			&& command.base != "shutdown" && command.base != "exit") {

			std::string output = "Unknown command.\n";
			size_t written;
			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Unknown_Error);
			kiv_os_rtl::Write_File(out, output.data(), output.size(), written);
			return;
		}
		else {
			if (command.base == "cd") {
				cd(command.parameters.data(), out);
				return;
			}
			else {
				kiv_os::THandle handle;
				kiv_os::THandle in_handle = in;
				kiv_os::THandle out_handle = out;
				kiv_os::THandle pipe_handles[2];

				if (command.is_red_in) {
					kiv_os_rtl::Open_File(command.file_name.data(), kiv_os::NOpen_File::fmOpen_Always, kiv_os::NFile_Attributes::System_File, in_handle);
				}

				if (command.is_red_out) {
					kiv_os_rtl::Open_File(command.file_name.data(), static_cast<kiv_os::NOpen_File>(0), kiv_os::NFile_Attributes::System_File, out_handle);
				}

				if (command.is_pipe) {
					kiv_os_rtl::Create_Pipe(pipe_handles);
					pipes_in.insert(std::pair<size_t, kiv_os::THandle>(current_command_position, pipe_handles[0]));
					pipes_out.insert(std::pair<size_t, kiv_os::THandle>(current_command_position, pipe_handles[1]));
					out_handle = pipe_handles[1];
				}

				if (current_command_position > 0 && pipes_in.find(current_command_position - 1) != pipes_in.end()) {
					in_handle = pipes_in[current_command_position - 1];
				}

				// Create process for new command.
				kiv_os_rtl::Create_Process(command.base.data(), command.parameters.data(), in_handle, out_handle, handle);

				handless.push_back(handle);
				current_command_position++;
			}
		}
	}

	// Wait for commands to be executed.
	kiv_os::THandle signalized_handler;
	size_t j = 0;

	for (std::vector<kiv_os::THandle>::iterator it = handless.begin(); it != handless.end(); ++it) {

		kiv_os::THandle single_handle[1];
		single_handle[0] = *it;

		
		kiv_os_rtl::Wait_For(single_handle, 1, signalized_handler);

		if (pipes_out.find(j) != pipes_out.end()) {
			kiv_os_rtl::Close_Handle(pipes_out[j]);
		}

		uint16_t exit_code = 0;
		kiv_os_rtl::Read_Exit_Code(signalized_handler, exit_code);
		j++;
	}

	for (size_t i = 0; i < pipes_in.size(); i++) {
		kiv_os_rtl::Close_Handle(pipes_in[i]);
	}

	pipes_in.clear();
	pipes_out.clear();
	handless.clear();
}