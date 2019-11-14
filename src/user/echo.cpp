#include "echo.h"
#include "rtl.h"
#include "global.h"

#include <string>

size_t __stdcall echo(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	std::string output = "";

	if (strlen(arguments) == 0) {
		if (echo_on) {
			// TODO echo: Use \n in string.
			output.append("Echo is on.");

			size_t written;
			kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
			kiv_os_rtl::Exit(exit_code);
			return 0;
		}
		else {
			// TODO echo: Use \n in string.
			output.append("Echo is off.");

			size_t written;
			kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

			uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
			kiv_os_rtl::Exit(exit_code);
			return 0;
		}
	}
	// TODO echo: /? didnt work.
	else if (strcmp(arguments, ECHO_HELP) == 0) {
		output.append("Displays messages, or turns command echoing off.\n");
		output.append("  ECHO [ON | OFF]\n");
		output.append("  ECHO [message]\n");
		// TODO echo: Use \n in string.
		output.append("Type ECHO without parameters to display the current echo setting.");

		size_t written;
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}
	else if (strcmp(arguments, ECHO_ON) == 0) {
		echo_on = true;

		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}
	else if (strcmp(arguments, ECHO_OFF) == 0) {
		echo_on = false;

		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}
	else {
		size_t written;
		// TODO echo: Use \n in string.
		output.append(arguments);
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}
}