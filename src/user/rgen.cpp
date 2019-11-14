#include "freq.h"
#include "rtl.h"
#include "time.h"

#include <string>

bool eof = false;
bool terminated = false;

size_t Terminated_Checker(const kiv_hal::TRegisters &regs) {
	terminated = true;
	return 0;
}

size_t __stdcall rgen(const kiv_hal::TRegisters &regs) {
	kiv_os::NSignal_Id signal = kiv_os::NSignal_Id::Terminate;
	kiv_os::TThread_Proc handler = reinterpret_cast<kiv_os::TThread_Proc>(Terminated_Checker);

	kiv_os_rtl::Register_Signal_Handler(signal, handler);

	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	std::string output;
	size_t written;

	// TODO rgen: When i use only "rgen", this condition is used.
	if (strlen(arguments) != 0) {
		// TODO rgen: Use \n in string.
		output = "Wrong arguments";
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Invalid_Argument);
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}

	srand(static_cast <unsigned> (time(0)));

	// kiv_os_rtl::Clone_Thread(name, arguments, std_in, std_out, handle); jine parametry, Terminated_Checker

	while (!eof && !terminated) {
		float ran_number = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)); // placeholder
		output = std::to_string(ran_number);

		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	}

	// kiv_os_rtl::Read_Exit_Code(handle);
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}

size_t Eof_Checker(const kiv_hal::TRegisters &regs) {
	kiv_os::NSignal_Id signal = kiv_os::NSignal_Id::Terminate;
	kiv_os::TThread_Proc handler = reinterpret_cast<kiv_os::TThread_Proc>(Terminated_Checker);

	// create thread nejspis preda data

	kiv_os_rtl::Register_Signal_Handler(signal, handler);

	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);

	char buffer[64];
	size_t read;

	kiv_os_rtl::Read_File(std_in, buffer, sizeof(buffer), read);

	while (read && !terminated) {
		kiv_os_rtl::Read_File(std_in, buffer, sizeof(buffer), read);
	}

	eof = true;
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);

	// TODO return exit code.
	return 0;
}