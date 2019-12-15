#pragma once

#include "kernel.h"

HMODULE User_Programs;
std::unique_ptr<IO> io;

kiv_os::THandle std_in_shell;
kiv_os::THandle std_out_shell;

void Initialize_Kernel() 
{
	User_Programs = LoadLibraryW(L"user.dll");
}

void Shutdown_Kernel() 
{
	FreeLibrary(User_Programs);
}

kiv_hal::TRegisters Prepare_SysCall_Context(kiv_os::NOS_Service_Major major, uint8_t minor)
{
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(major);
	regs.rax.l = minor;
	return regs;
}

kiv_os::THandle Create_Kernel_Process() 
{
	char name[7] = "kernel";
	// Create process and thread.
	std::unique_ptr<Process> process = std::make_unique<Process>(io->io_process->Get_Free_Process_ID(), name);
	std::unique_ptr<Thread> thread = std::make_unique<Thread>(process->process_ID);
	thread->thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t thread_ID = thread->thread_ID;
	kiv_os::THandle kernel_handler = io->io_process->Get_Free_Thread_ID();

	// Set attributes.
	process->state = State::Running;
	process->process_thread_ID = thread_ID;
	process->working_dir.push_back(io->vfs->mft_items[0]);

	process->threads.insert(std::pair<size_t, std::unique_ptr<Thread>>(thread->thread_ID, std::move(thread)));

	size_t process_ID = process->process_ID;

	io->io_process->t_handle_to_thread_ID.insert(std::pair<kiv_os::THandle, size_t>(kernel_handler, thread_ID));
	io->io_process->thread_ID_to_process_ID.insert(std::pair<size_t, size_t>(thread_ID, process->process_ID));
	io->io_process->processes.insert(std::pair<size_t, std::unique_ptr<Process>>(process->process_ID, std::move(process)));

	return kernel_handler;
}

void Remove_Kernel_Process(kiv_os::THandle kernel_handler) 
{
	size_t thread_ID = io->io_process->t_handle_to_thread_ID[kernel_handler];
	size_t process_ID = io->io_process->thread_ID_to_process_ID[thread_ID];

	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Exit));
	regs.rcx.x = static_cast<decltype(regs.rcx.x)>(kiv_os::NOS_Error::Success);

	io->io_process->Exit(regs);

	regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	regs.rdx.x = kernel_handler;

	io->io_process->Read_Exit_Code(regs);
}

kiv_os::THandle Shell_Clone() 
{
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>("shell");
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>("");

	regs.rbx.e = (std_in_shell << 16) | std_out_shell;

	// Create shell process.
	io->io_process->Create_Process(regs);
	return static_cast<kiv_os::THandle>(regs.rax.x);
}

void Shell_Wait(kiv_os::THandle handle) 
{
	// Wait for shell to be closed.

	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(&handle);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(1);

	io->io_process->Handle_Process(regs);
}

void Shell_Close(kiv_os::THandle shell_handle)
{
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));

	// Close std_in handle.
	regs.rdx.x = static_cast<decltype(regs.rdx.r)>(std_in_shell);
	io->Handle_IO(regs);

	// Close std_out handle.
	regs.rdx.x = static_cast<decltype(regs.rdx.r)>(std_out_shell);
	io->Handle_IO(regs);

	// Delete shell process.
	regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	regs.rdx.x = static_cast<decltype(regs.rdx.r)>(shell_handle);
	io->io_process->Handle_Process(regs);
}

void __stdcall Sys_Call(kiv_hal::TRegisters &regs)
{

	switch (static_cast<kiv_os::NOS_Service_Major>(regs.rax.h)) 
	{

		case kiv_os::NOS_Service_Major::File_System:
			io->Handle_IO(regs);
			break;

		case kiv_os::NOS_Service_Major::Process:
			io->io_process->Handle_Process(regs);
			break;
	}

}

void __stdcall Bootstrap_Loader(kiv_hal::TRegisters &context)
{
	printf("initialize kernel\n");
	Initialize_Kernel();
	printf("end_initialize kernel\n");
	kiv_hal::Set_Interrupt_Handler(kiv_os::System_Int_Number, Sys_Call);
	printf("handler setted\n");
	kiv_hal::TRegisters regs;

	std_in_shell = Convert_Native_Handle(new STD_Handle_In());
	std_out_shell = Convert_Native_Handle(new STD_Handle_Out());
	printf("std in handlers created\n");

	for (regs.rdx.l = 0; ; regs.rdx.l++) 
	{

		// Get Drive parameters.
		kiv_hal::TDrive_Parameters params;
		regs.rax.h = static_cast<uint8_t>(kiv_hal::NDisk_IO::Drive_Parameters);
		regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&params);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
		
		if (!regs.flags.carry) 
		{

			// Load boot block.
			uint16_t bytes_per_sector = params.bytes_per_sector;
			uint64_t number_of_sectors = params.absolute_number_of_sectors;

			io = std::make_unique<IO>(number_of_sectors, bytes_per_sector, regs.rdx.l);
			printf("io created\n");
			break;
		}

		if (regs.rdx.l == 255)
		{
			break;
		}
	}
	

	kiv_os::THandle kernel_handler = 0;
	printf("create kernel process\n");
	kernel_handler = Create_Kernel_Process();	

	// Create shell.
	printf("clone shell\n");
	kiv_os::THandle handle = Shell_Clone();
	printf("shell wait\n");
	// Wait for shell to be closed.
	Shell_Wait(handle);

	// Close std_in and std_out handles + destroy shell process.
	Shell_Close(handle);

	Remove_Kernel_Process(kernel_handler);

	// Shutdown kernel.
	Shutdown_Kernel();
}


void Set_Error(const bool failed, kiv_hal::TRegisters &regs) 
{
	if (failed)
	{
		regs.flags.carry = true;
		regs.rax.r = GetLastError();
	}
	else
	{
		regs.flags.carry = false;
	}
}



