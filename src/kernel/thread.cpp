#include "thread.h"

std::condition_variable condition;

size_t Get_Thread_ID(std::thread::id thread_ID) {
	return std::hash<std::thread::id>()(thread_ID);
}

Thread::Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers) {
	thread_ID = 0;
	state = State::Ready;
	registers = t_registers;
	exit_code = 0;
	entry_point = t_entry_point;
}

Thread::~Thread() {
	handlers.clear();

	if (std_thread.joinable()) {
		std_thread.detach();
	}
}

void Thread::Start() {
	state = State::Running;
	std_thread = std::thread(entry_point, registers);
	thread_ID = Get_Thread_ID(std_thread.get_id());
}

void Thread::Join(uint32_t t_exit_code) {
	state = State::Exited;
	exit_code = t_exit_code;
	std_thread.join();
}