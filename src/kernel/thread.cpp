#include "thread.h"

std::condition_variable condition;

size_t Thread::Get_Thread_ID(std::thread::id thread_ID) 
{
	return std::hash<std::thread::id>()(thread_ID);
}

Thread::Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers, size_t t_parent_ID) 
{
	thread_ID = 0;
	parent_ID = t_parent_ID;
	state = State::Runnable;
	registers = t_registers;
	exit_code = -1;
	entry_point = t_entry_point;
	waked_by_handler = 0;
}

Thread::Thread(size_t t_parent_ID) 
{
	state = State::Running;
	parent_ID = t_parent_ID;
	exit_code = -1;
	waked_by_handler = 0;
	thread_ID = 0;

	kiv_hal::TRegisters regs;
	regs.rax.x = 0;
	registers = regs;

	entry_point = nullptr;
}

void Thread::Start() 
{
	state = State::Running;
	printf("start %I64d\n", registers.rdi.r);
	const char* file_name = reinterpret_cast<const char*>(registers.rdi.r);
	printf("start %s\n", file_name);
	std_thread = std::thread(entry_point, registers);
	thread_ID = Get_Thread_ID(std_thread.get_id());
}

void Thread::Join(uint16_t t_exit_code) 
{

	exit_code = t_exit_code;
	state = State::Exited;
	
	if (std_thread.joinable())
	{
		std_thread.detach();
	}
}

void Thread::Stop() 
{	
	std::unique_lock<std::mutex> lock(mutex);
	if (handlers_waiting_for.size() > 0)
	{

		if (state == State::Running) 
		{
			state = State::Blocked;

			cv.wait(lock);
		}
	}
}

void Thread::Remove_Handler_From_Handlers_Waiting_For(size_t thread_ID) 
{
	std::unique_lock<std::mutex> lock(mutex);

	if (handlers_waiting_for[thread_ID] == 0)
	{
		cv.notify_all();

		if (state == State::Blocked) 
		{
			state = State::Running;
			waked_by_handler = thread_ID;
		}
	}
	lock.unlock();
}