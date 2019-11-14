#include "thread.h"

std::condition_variable condition;

size_t Get_Thread_ID(std::thread::id thread_ID) {
	return std::hash<std::thread::id>()(thread_ID);
}

Thread::Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers) {
	thread_ID = 0;
	state = State::Runnable;
	registers = t_registers;
	exit_code = 0;
	entry_point = t_entry_point;
}

Thread::~Thread() {
	terminate_handlers.clear();
	if (std_thread.joinable()) {
		std_thread.detach();
	}
}

void Thread::Start() {
	state = State::Running;
	std_thread = std::thread(entry_point, registers);
	thread_ID = Get_Thread_ID(std_thread.get_id());
}

void Thread::Stop(uint16_t t_exit_code) {
	state = State::Blocked;
	exit_code = t_exit_code;
}

void Thread::Join() {
	state = State::Exited;

	std::map<kiv_os::NSignal_Id, kiv_os::TThread_Proc>::iterator it_handler = terminate_handlers.begin();

	kiv_hal::TRegisters terminate_registers;
	terminate_registers.rcx.l = static_cast<decltype(terminate_registers.rcx.l)>(kiv_os::NSignal_Id::Terminate);

	while (it_handler != terminate_handlers.end()) {

		// Call terminate handler.
		it_handler->second(terminate_registers);

		it_handler++;
	}
	
	//std_thread.join();
}