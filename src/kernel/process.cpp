#include "process.h"

Process::Process(size_t p_process_ID, const char *p_name, std::vector<Mft_Item*> p_working_dir, kiv_os::THandle p_handle_in, kiv_os::THandle p_handle_out)
{
	std::string str = std::string(p_name);
	name = std::vector<char>(str.length());
	memcpy(name.data(), p_name, str.length());
	name.push_back('\0');

	process_ID = p_process_ID;
	state = State::Runnable;
	working_dir = p_working_dir;
	handle_in = p_handle_in;
	handle_out = p_handle_out;
	process_thread_ID = 0;
}

Process::Process(size_t p_process_ID, const char *p_name)
{
	std::string str = std::string(p_name);
	name = std::vector<char>(str.length());
	memcpy(name.data(), p_name, str.length());
	name.push_back('\0');
	process_ID = p_process_ID;
	state = State::Runnable;

	handle_in = 0;
	handle_out = 0;
	process_thread_ID = 0;
}


size_t Process::Create_Thread(kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers, const char* arguments)
{
	std::unique_ptr<Thread> thread = std::make_unique<Thread>(entry_point, registers, arguments, process_ID);
	thread->Start();

	state = State::Running;

	size_t thread_ID = thread->thread_ID;
	threads.insert(std::pair<size_t, std::unique_ptr<Thread>>(thread->thread_ID, std::move(thread)));
	return thread_ID;
}

// Terminate all handlers and join std thread.
void Process::Join_Thread(size_t thread_ID, uint16_t exit_code)
{
	threads[thread_ID]->Join(exit_code);

	if (process_thread_ID == thread_ID) {
		state = State::Exited;
	}
}