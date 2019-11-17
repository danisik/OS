#pragma once

#include "kernel.h"

HMODULE User_Programs;
IO_Process *io_process = new IO_Process();

kiv_os::THandle std_in_shell;
kiv_os::THandle std_out_shell;


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

void Create_Kernel_Process() {
	
	char *working_directory = "";
	// Create process and thread.
	std::unique_ptr<Process> process = std::make_unique<Process>(io_process->Get_Free_Process_ID(), working_directory);
	std::unique_ptr<Thread> thread = std::make_unique<Thread>(process->process_ID);
	thread->thread_ID = Get_Thread_ID(std::this_thread::get_id());
	size_t thread_ID = thread->thread_ID;

	// Set states.
	process->state = State::Running;

	process->threads.insert(std::pair<size_t, std::unique_ptr<Thread>>(thread->thread_ID, std::move(thread)));

	size_t process_ID = process->process_ID;

	io_process->t_handle_to_thread_ID.insert(std::pair<kiv_os::THandle, size_t>(io_process->Get_Free_Thread_ID(), thread_ID));
	io_process->thread_ID_to_process_ID.insert(std::pair<size_t, size_t>(thread_ID, process->process_ID));
	io_process->processes.insert(std::pair<size_t, std::unique_ptr<Process>>(process->process_ID, std::move(process)));
}

kiv_os::THandle Shell_Clone() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>("shell");
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>("");

	regs.rbx.e = (std_in_shell << 16) | std_out_shell;

	// Create shell process.
	io_process->Clone_Process(regs);
	return static_cast<kiv_os::THandle>(regs.rax.x);
}

void Shell_Wait(kiv_os::THandle handle) {
	// Wait for shell to be closed.

	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(&handle);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(1);

	io_process->Handle_Process(regs);
}

void Shell_Close(kiv_os::THandle shell_handle, kiv_os::THandle std_in, kiv_os::THandle std_out) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));
	
	// Close std_in handle.
	regs.rdx.x = static_cast<decltype(regs.rdx.r)>(std_in);
	Handle_IO(regs);

	// Close std_out handle.
	regs.rdx.x = static_cast<decltype(regs.rdx.r)>(std_out);
	Handle_IO(regs);

	// Delete shell process.
	regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	regs.rdx.x = static_cast<decltype(regs.rdx.r)>(shell_handle);
	io_process->Handle_Process(regs);
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

	Create_Kernel_Process();
	
	// regs.rax.h = static_cast<uint8_t>(kiv_hal::NDisk_IO::Drive_Parameters); -> stdin 1, stdout 52428.
	std_in_shell = regs.rax.x;
	std_out_shell = regs.rbx.x;

	// Create shell.
	kiv_os::THandle handle = Shell_Clone();

	// Wait for shell to be closed.
	Shell_Wait(handle);

	// Close std_in and std_out handles + destroy shell process.
	Shell_Close(handle, std_in_shell, std_out_shell);

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



