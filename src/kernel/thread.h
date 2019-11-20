#pragma once

#include "../api/api.h"
#include "state.h"

#include <thread>
#include <map>
#include <condition_variable>

class Thread {
public:
	size_t thread_ID;
	size_t parent_ID;
	State state;
	std::thread std_thread;
	kiv_hal::TRegisters registers;
	uint16_t exit_code;
	kiv_os::TThread_Proc entry_point;
	std::map<kiv_os::NSignal_Id, kiv_os::TThread_Proc> terminate_handlers;
	std::map<size_t, size_t> handlers_waiting_for;
	std::map<size_t, size_t> sleeped_handlers;

	std::condition_variable cv;
	std::mutex mutex;
	size_t waked_by_handler;

	Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers, size_t t_parent_ID);
	Thread(size_t t_parent_ID);
	~Thread();
	void Start();
	void Join(uint16_t t_exit_code);
	void Stop();
	void Remove_Handler_From_Handlers_Waiting_For(size_t thread_ID);
	static size_t Get_Thread_ID(std::thread::id thread_ID);
};