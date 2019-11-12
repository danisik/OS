#include "io_process.h"



size_t IO_Process::Get_Free_Process_ID() {

	while (1) {
		first_free_process_ID++;
		if (processes.find(first_free_process_ID) == processes.end()) {
			return first_free_process_ID;
		}
	}
}

void IO_Process::Clone(kiv_hal::TRegisters &regs) {

	kiv_os::NClone clone_type = static_cast<kiv_os::NClone>(regs.rcx.r);

	switch (clone_type) {
		case kiv_os::NClone::Create_Process:
			Clone_Process(regs);
			break;
		case kiv_os::NClone::Create_Thread:
			Clone_Thread(regs);
			break;
	}
}

void IO_Process::Clone_Process(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);

	char *export_name = reinterpret_cast<char*>(regs.rdx.r);
	char *arguments = reinterpret_cast<char*>(regs.rdi.r);

	// TODO Clone_Process: working directory.
	char *working_directory = "";

	kiv_os::TThread_Proc entry_point = (kiv_os::TThread_Proc)GetProcAddress(User_Programs, export_name);

	//   |stdin|stdout| in hex
	//    |....|....| 
	//	  
	//	 stdin  = rbx.e >> 16
	//
	//					  |....|....|
	//   stdout = rbx.e & |0000|FFFF|
	//	

	kiv_hal::TRegisters process_registers;
	process_registers.rax.x = regs.rbx.e >> 16;			// Stdin.
	process_registers.rbx.x = regs.rbx.e & 0x0000FFFF;	// Stdout.
	process_registers.rdi.r = regs.rdi.r;				// Arguments.

	std::unique_ptr<Process> process = std::make_unique<Process>(Get_Free_Process_ID(), working_directory, entry_point, process_registers);

	// Create first thread.
	size_t thread_ID = process->Create_Thread(entry_point, process_registers);

	//Add newly created process into "processes" map.
	processes.insert(std::pair<size_t, std::unique_ptr<Process>>(thread_ID, std::move(process)));

	regs.rax.x = static_cast<kiv_os::THandle>(process->process_ID);
}	

void IO_Process::Clone_Thread(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);

	kiv_os::TThread_Proc entry_point = reinterpret_cast<kiv_os::TThread_Proc>(regs.rdx.r);

	kiv_hal::TRegisters thread_registers;
	thread_registers.rdi.r = regs.rdi.r;				// Arguments.

	// Get current process and create new thread.
	size_t current_thread_ID = Get_Thread_ID(std::this_thread::get_id());
	std::unique_ptr<Process> current_process = std::move(processes.find(current_thread_ID)->second);

	size_t cloned_thread_ID = current_process->Create_Thread(entry_point, thread_registers);

	regs.rax.x = static_cast<kiv_os::THandle>(cloned_thread_ID);
}

void IO_Process::Wait_For(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);
	// TODO Wait_For: functional code.

	//IN : rdx pointer na pole THandle, na ktere se ma cekat, rcx je pocet handlu
	//	funkce se vraci jakmile je signalizovan prvni handle
	//OUT : rax je index handle, ktery byl signalizovan
}

void IO_Process::Read_Exit_Code(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);
	// TODO Read_Exit_Code: functional code.

	//IN:  dx je handle procesu/thread jehoz exit code se ma cist
	//OUT: cx je exitcode
}

void IO_Process::Exit(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);
	// TODO Exit: functional code.

	//ukonci proces/vlakno
	//IN: cx je exit code
}

void IO_Process::Shutdown(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);
	// TODO Shutdown: functional code.

	//nema parametry, nejprve korektne ukonci vsechny bezici procesy a pak kernel, cimz se preda rizeni do boot.exe, ktery provede simulaci vypnuti pocitace pres ACPI
}

void IO_Process::Register_Signal_Handler(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);
	// TODO Register_Signal_Handler: functional code.

	//IN: rcx NSignal_Id, rdx 
	//	a) pointer na TThread_Proc, kde pri jeho volani context.rcx bude id signalu
	//	b) 0 a pak si OS dosadi defualtni obsluhu signalu
}


void IO_Process::Handle_Process(kiv_hal::TRegisters &regs) {

	switch (static_cast<kiv_os::NOS_Process>(regs.rax.l)) {

	case kiv_os::NOS_Process::Clone:
		Clone(regs);
		break;

	case kiv_os::NOS_Process::Wait_For:
		Wait_For(regs);
		break;

	case kiv_os::NOS_Process::Read_Exit_Code:
		Read_Exit_Code(regs);
		break;

	case kiv_os::NOS_Process::Exit:
		Exit(regs);
		break;

	case kiv_os::NOS_Process::Shutdown:
		Shutdown(regs);
		break;

	case kiv_os::NOS_Process::Register_Signal_Handler:
		Register_Signal_Handler(regs);
		break;
	}

}