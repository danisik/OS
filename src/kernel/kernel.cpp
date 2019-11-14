#pragma once

#include "kernel.h"

HMODULE User_Programs;
IO_Process *io_process;


void Initialize_Kernel() {
	io_process = new IO_Process();
	User_Programs = LoadLibraryW(L"user.dll");
}

void Shutdown_Kernel() {
	FreeLibrary(User_Programs);
}

kiv_hal::TRegisters Prepare_SysCall_Context(kiv_os::NOS_Service_Major major, uint8_t minor) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(major);
	regs.rax.l = minor;
	return regs;
}

kiv_os::THandle Shell_Clone() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>("shell");
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>("");

	// TODO Shell_Clone: Get in out handles.
	//const auto std_handle = Register_STD();
	//kiv_os::THandle stdin_handle = std_handle.in;
	//kiv_os::THandle stdout_handle = std_handle.out;
	//regs.rbx.e = (stdin_handle << 16) | stdout_handle;

	// Create shell process.
	io_process->Clone_Process(regs);
	return static_cast<kiv_os::THandle>(regs.rax.x);
}

void Shell_Wait(kiv_os::THandle handle) {
	// Wait for shell to be closed.

	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));
	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(handle);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(1);

	Handle_IO(regs);
}

void Shell_Close(kiv_os::THandle std_in, kiv_os::THandle std_out) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));
	
	// Close std_in handle.
	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(std_in);
	Handle_IO(regs);

	// Close std_out handle.
	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(std_out);
	Handle_IO(regs);
}

void __stdcall Sys_Call(kiv_hal::TRegisters &regs) {

	switch (static_cast<kiv_os::NOS_Service_Major>(regs.rax.h)) {

	case kiv_os::NOS_Service_Major::File_System:
		Handle_IO(regs);
		break;

	case kiv_os::NOS_Service_Major::Process:
		io_process->Handle_Process(regs);
		break;
	}

}

void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context) {
	Initialize_Kernel();
	kiv_hal::Set_Interrupt_Handler(kiv_os::System_Int_Number, Sys_Call);

	kiv_hal::TRegisters regs;
	for (regs.rdx.l = 0; ; regs.rdx.l++) {
		kiv_hal::TDrive_Parameters params;
		regs.rax.h = static_cast<uint8_t>(kiv_hal::NDisk_IO::Drive_Parameters);
		regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&params);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);

		if (!regs.flags.carry) {

		}

		if (regs.rdx.l == 255) break;
	}

	//spustime shell - v realnem OS bychom ovsem spousteli login

	kiv_os::TThread_Proc shell = (kiv_os::TThread_Proc)GetProcAddress(User_Programs, "shell");

	if (shell) {
		//spravne se ma shell spustit pres clone!
		//ale ten v kostre pochopitelne neni implementovan		
		shell(regs);
	}

	// Create shell.
	//kiv_os::THandle handle = Shell_Clone();
	
	// Wait for shell to be closed.
	//Shell_Wait(handle)

	// Close std_in and std_out handles.
	//Shell_Close(std_in, std_out);

	// Shutdown kernel.
	Shutdown_Kernel();
}


void Set_Error(const bool failed, kiv_hal::TRegisters &regs) {
	if (failed) {
		regs.flags.carry = true;
		regs.rax.r = GetLastError();
	}
	else
		regs.flags.carry = false;
}



