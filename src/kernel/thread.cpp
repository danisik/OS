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
	exit_code = 0;
	entry_point = t_entry_point;
}

Thread::~Thread() {

}

void Thread::Start() {
	state = State::Running;
	std_thread = std::thread(entry_point, registers);
	thread_ID = Get_Thread_ID(std_thread.get_id());
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

	terminate_handlers.clear();
	std_thread.join();
}

void Thread::Stop() {
	printf("Stop: started %zd\n", thread_ID);
	if (handlers_waiting_for.size() > 0) {

		if (state == State::Running) {
			state = State::Blocked;

			std::unique_lock<std::mutex> lock(mutex);
			printf("Stop: Locking %zd\n", thread_ID);
			cv.wait(lock);
		}
	}
	printf("Stop: ended %zd\n", thread_ID);
}

void Thread::Add_Handlers_Waiting_For(size_t thread_ID) {

	printf("Add_Handlers_Waiting_For: %zd\n", thread_ID);
	handlers_waiting_for.insert(std::pair<size_t, size_t>(thread_ID, thread_ID));

	printf("Add_Handlers_Waiting_For: %zd ended.\n", thread_ID);
}

void Thread::Remove_Handler_From_Handlers_Waiting_For(size_t thread_ID) {
	printf("Remove_Handler_From_Handlers_Waiting_For: %zd\n", thread_ID);
	handlers_waiting_for.erase(thread_ID);

	if (handlers_waiting_for.size() == 0) {
		state = State::Running;

		std::unique_lock<std::mutex> lock(mutex);
		lock.unlock();
		printf("Remove_Handler_From_Handlers_Waiting_For: notyfiing %zd\n", thread_ID);
		cv.notify_all();
	}
	printf("Remove_Handler_From_Handlers_Waiting_For: ending %zd\n", thread_ID);
}