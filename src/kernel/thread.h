#pragma once

#include "../api/api.h"
#include "state.h"

#include <thread>
#include <map>
#include <condition_variable>

class Thread {
public:
	size_t thread_ID;
	State state;
	std::thread std_thread;
	kiv_hal::TRegisters registers;
	uint32_t exit_code;
	kiv_os::TThread_Proc entry_point;
	std::map<kiv_os::NSignal_Id, kiv_os::TThread_Proc> terminate_handlers;

	Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers);
	~Thread();
	void Start();
	void Stop(uint16_t t_exit_code);
	void Join();
};