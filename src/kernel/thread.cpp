#include "thread.h"

std::condition_variable condition;

size_t Get_Thread_ID(std::thread::id thread_ID) {
	return std::hash<std::thread::id>()(thread_ID);
}

Thread::Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers, size_t t_parent_ID) {
	thread_ID = 0;
	parent_ID = t_parent_ID;
	state = State::Runnable;
	registers = t_registers;
	exit_code = -1;
	entry_point = t_entry_point;
}

Thread::Thread(size_t t_parent_ID) {
	state = State::Running;
	parent_ID = t_parent_ID;
	exit_code = -1;
}

Thread::~Thread() {

}

void Thread::Start() {
	state = State::Running;
	std_thread = std::thread(entry_point, registers);
	thread_ID = Get_Thread_ID(std_thread.get_id());
}

void Thread::notify_All() {
	for (std::vector<std::condition_variable>::iterator it = condition_variables.begin(); it != condition_variables.end(); ++it) {
		it->notify_one();
	}
}

void Thread::Join(uint16_t t_exit_code) {

	exit_code = t_exit_code;
	state = State::Exited;

	std::map<kiv_os::NSignal_Id, kiv_os::TThread_Proc>::iterator it_handler = terminate_handlers.begin();

	kiv_hal::TRegisters terminate_registers;
	terminate_registers.rcx.l = static_cast<decltype(terminate_registers.rcx.l)>(kiv_os::NSignal_Id::Terminate);

	while (it_handler != terminate_handlers.end()) {

		// Call terminate handler.
		it_handler->second(terminate_registers);

		it_handler++;
	}

	notify_All();
	
	terminate_handlers.clear();
	sleeped_handlers.clear();
	
	if (std_thread.joinable()) {
		std_thread.detach();
	}
}

void Thread::Stop() {	
	std::unique_lock<std::mutex> lock(mutex);
	if (handlers_waiting_for.size() > 0) {

		if (state == State::Running) {
			state = State::Blocked;

			cv.wait(lock);
		}
	}
}

void Thread::Remove_Handler_From_Handlers_Waiting_For(size_t thread_ID) {
	std::unique_lock<std::mutex> lock(mutex);
	handlers_waiting_for.clear();

	state = State::Running;

	cv.notify_all();

	waked_by_handler = thread_ID;

	lock.unlock();
}