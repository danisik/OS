#include "process.h"

Process::Process(size_t p_process_ID, char *p_name, char *p_working_directory) {
	process_ID = p_process_ID;
	name = p_name;
	state = State::Runnable;
	working_directory = p_working_directory;
}


size_t Process::Create_Thread(kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers) {
	std::unique_ptr<Thread> thread = std::make_unique<Thread>(entry_point, registers, process_ID);
	thread->Start();

	state = State::Running;
	
	size_t thread_ID = thread->thread_ID;
	threads.insert(std::pair<size_t, std::unique_ptr<Thread>>(thread->thread_ID, std::move(thread)));
	return thread_ID;
}

// Terminate all handlers and join std thread.
void Process::Join_Thread(size_t thread_ID, uint16_t exit_code) {
	threads[thread_ID]->Join(exit_code);

	if (process_thread_ID == thread_ID) {
		state = State::Exited;
	}
}

uint16_t Process::Kill_Thread(size_t thread_ID) {
	// Pass object from array into object. Object will be destroyed after exiting method (property of unique_ptr).
	std::unique_ptr<Thread> thread = std::move(threads[thread_ID]);
	
	return thread->exit_code;
}