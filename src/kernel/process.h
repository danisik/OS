#pragma once

#include "thread.h"

#include <map>


class Process {
public:
	size_t process_ID;
	State state;
	char *working_directory;
	std::map<size_t, std::unique_ptr<Thread>> threads;

	Process(size_t p_process_ID, char *p_working_directory, kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers);
	size_t Create_Thread(kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers);
	void Join_Thread(size_t thread_ID, uint32_t exit_code);
};

size_t Get_Thread_ID(std::thread::id thread_ID);
