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
	processes.insert(std::pair<size_t, std::unique_ptr<Process>>(process->process_ID, std::move(process)));
	thread_ID_to_process_ID.insert(std::pair<size_t, size_t>(thread_ID, process->process_ID));

	regs.rax.x = static_cast<kiv_os::THandle>(process->process_ID);
}	

void IO_Process::Clone_Thread(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);

	kiv_os::TThread_Proc entry_point = reinterpret_cast<kiv_os::TThread_Proc>(regs.rdx.r);

	kiv_hal::TRegisters thread_registers;
	thread_registers.rdi.r = regs.rdi.r;				// Arguments.

	// Get current process and create new thread.
	size_t current_thread_ID = Get_Thread_ID(std::this_thread::get_id());
	size_t process_ID = thread_ID_to_process_ID.find(current_thread_ID)->second;
	std::unique_ptr<Process> current_process = std::move(processes.find(process_ID)->second);

	size_t cloned_thread_ID = current_process->Create_Thread(entry_point, thread_registers);
	thread_ID_to_process_ID.insert(std::pair<size_t, size_t>(cloned_thread_ID, current_process->process_ID));

	regs.rax.x = static_cast<kiv_os::THandle>(cloned_thread_ID);
}

void IO_Process::Wait_For(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);

	kiv_os::THandle *handles = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);
	size_t handles_count = static_cast<size_t>(regs.rcx.r);

	kiv_os::THandle handle;
	size_t process_ID;
	std::unique_ptr<Process> current_process;
	std::unique_ptr<Thread> current_thread;

	std::vector<Thread*> wait_threads(handles_count);
	
	for (int i = 0; i < handles_count; i++) {
		handle = handles[i];

		process_ID = thread_ID_to_process_ID.find(handle)->second;
		current_process = std::move(processes.find(process_ID)->second);
		current_thread = std::move(current_process->threads.find(handle)->second);

		wait_threads[i] = current_thread.get();
	}

	while (1)
	{
		for (int i = 0; i < handles_count; i++)
		{
			Thread *checked_thread = wait_threads[i];
			
			if (checked_thread->state == State::Exited)
			{
				regs.rax.r = static_cast<decltype(regs.rax.r)>(checked_thread->thread_ID);
				return;
			}
		}
	}

	return;
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
	// TODO EXit: send terminate signal to all thread.

	//ukonci proces/vlakno
	//IN: cx je exit code
}

void IO_Process::Shutdown(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);

	std::unique_ptr<Process> process;
	std::map<size_t, std::unique_ptr<Process>>::iterator it_process = processes.begin();
	std::map<size_t, std::unique_ptr<Thread>>::iterator it_thread;

	while (it_process != processes.end()) {
		process = std::move(it_process->second);

		it_thread = process->threads.begin();

		while (it_thread != process->threads.end()) {
			process->Join_Thread(it_thread->first);

			it_thread++;
		}

		it_process++;	
	}
}

void IO_Process::Register_Signal_Handler(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_process_mutex);

	kiv_os::NSignal_Id signal = static_cast<kiv_os::NSignal_Id>(regs.rcx.r);
	kiv_os::TThread_Proc process_handle = reinterpret_cast<kiv_os::TThread_Proc>(regs.rdx.r);

	size_t current_thread_ID = Get_Thread_ID(std::this_thread::get_id());
	size_t process_ID = thread_ID_to_process_ID.find(current_thread_ID)->second;
	std::unique_ptr<Process> current_process = std::move(processes.find(process_ID)->second);
	std::unique_ptr<Thread> current_thread = std::move(current_process->threads.find(current_thread_ID)->second);
	current_thread->terminate_handlers.insert(std::pair<kiv_os::NSignal_Id, kiv_os::TThread_Proc>(signal, process_handle));
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