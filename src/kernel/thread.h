#pragma once

#include "../api/api.h"
#include "State.h"

#include <thread>

enum class ThreadAction {
	Join = 1,
	Stop
};

class Thread {
	public:
		size_t thread_ID;
		State state;
		std::thread std_thread;
		kiv_hal::TRegisters registers;
		uint32_t exit_code;
		kiv_os::TThread_Proc entry_point;
		std::vector<kiv_os::TThread_Proc> handlers;

		Thread(kiv_os::TThread_Proc t_entry_point, kiv_hal::TRegisters t_registers);
		~Thread();
		void Start();
		void Join(uint32_t exitCode);
};