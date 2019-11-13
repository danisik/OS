#include "process.h"

Process::Process(size_t p_process_ID, char *p_working_directory, kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers) {
	process_ID = p_process_ID;
	state = State::Ready;
	working_directory = p_working_directory;	
}


size_t Process::Create_Thread(kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers) {
	std::unique_ptr<Thread> thread = std::make_unique<Thread>(entry_point, registers);
	thread->Start(); 

	if (state == State::Ready) {
		state = State::Running;
	}

	threads.insert(std::pair<size_t, std::unique_ptr<Thread>>(thread->thread_ID, std::move(thread)));

	return thread->thread_ID;
}

void Process::Stop_Thread(size_t thread_ID, uint16_t exit_code) {
	std::unique_ptr<Thread> thread = std::move(threads[thread_ID]);
	thread->Stop(exit_code);
}

void Process::Join_Thread(size_t thread_ID) {
	std::unique_ptr<Thread> thread = std::move(threads[thread_ID]);
	thread->Join();

	threads.erase(thread_ID);

	if (threads.size() < 1) {
		state = State::Blocked;
	}
}