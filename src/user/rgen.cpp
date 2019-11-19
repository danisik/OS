#include "rgen.h"

bool terminated = false;

size_t Terminated_Checker(const kiv_hal::TRegisters &regs) {
	terminated = true;
	return 0;
}

size_t Eof_Checker(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);

	bool *eof = reinterpret_cast<bool *>(regs.rdi.r);

	kiv_os::NSignal_Id signal = kiv_os::NSignal_Id::Terminate;
	kiv_os::TThread_Proc handler = reinterpret_cast<kiv_os::TThread_Proc>(Terminated_Checker);

	kiv_os_rtl::Register_Signal_Handler(signal, handler);

	char buffer[1];
	size_t read;

	kiv_os_rtl::Read_File(std_in, buffer, 1, read);

	while (read && !terminated) {
		kiv_os_rtl::Read_File(std_in, buffer, 1, read);
	}

	*eof = true;

	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}

size_t __stdcall rgen(const kiv_hal::TRegisters &regs) {	
	const kiv_os::THandle std_in = static_cast<kiv_os::THandle>(regs.rax.x);
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	const char *arguments = reinterpret_cast<const char *>(regs.rdi.r);

	kiv_os::NSignal_Id signal = kiv_os::NSignal_Id::Terminate;
	kiv_os::TThread_Proc handler = reinterpret_cast<kiv_os::TThread_Proc>(Terminated_Checker);

	kiv_os_rtl::Register_Signal_Handler(signal, handler);

	std::string output;
	size_t written;

	if (strlen(arguments) != 0) {
		output = "Wrong arguments.\n";
		uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Invalid_Argument);
		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
		kiv_os_rtl::Exit(exit_code);
		return 0;
	}

	srand(static_cast <unsigned> (time(0)));

	kiv_os::THandle handle;
	bool eof = false;

	kiv_os_rtl::Clone_Thread(&Eof_Checker, &eof, std_in, std_out, handle);

	while (!eof && !terminated) {
		float ran_number = static_cast <float> (rand());
		output = std::to_string(ran_number);
		output.append("\n");

		kiv_os_rtl::Write_File(std_out, output.data(), output.size(), written);
	}

	uint16_t checker_exit_code;

	if (terminated) {
		kiv_os::THandle *handles = &handle;
		kiv_os::THandle signalized_handle;
		kiv_os_rtl::Wait_For(handles, 1, signalized_handle);
	}
	
	kiv_os_rtl::Read_Exit_Code(handle, checker_exit_code);
	uint16_t exit_code = static_cast<uint16_t>(kiv_os::NOS_Error::Success);
	kiv_os_rtl::Exit(exit_code);
	return 0;
}