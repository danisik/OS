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
			output.append("Echo is on.");

			size_t written;
			kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

			kiv_os_rtl::Exit(0);
			return 0;
		}
		else {
			output.append("Echo is off.");

			size_t written;
			kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

			kiv_os_rtl::Exit(0);
			return 0;
		}
	}
	else if (strcmp(arguments, ECHO_HELP) == 0) {
		output.append("Displays messages, or turns command echoing off.\n");
		output.append("  ECHO [ON | OFF]\n");
		output.append("  ECHO [message]\n");
		output.append("Type ECHO without parameters to display the current echo setting.");

		size_t written;
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

		kiv_os_rtl::Exit(0);
		return 0;
	}
	else if (strcmp(arguments, ECHO_ON) == 0) {
		echo_on = true;

		kiv_os_rtl::Exit(0);
		return 0;
	}
	else if (strcmp(arguments, ECHO_OFF) == 0) {
		echo_on = false;

		kiv_os_rtl::Exit(0);
		return 0;
	}
	else {
		size_t written;
		output.append(arguments);
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);

		kiv_os_rtl::Exit(0);
		return 0;
	}
}