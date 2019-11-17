#pragma once

#include "thread.h"

#include <map>


class Process {
public:
	size_t process_ID;
	size_t process_thread_ID;
	State state;
	char *working_directory;
	std::map<size_t, std::unique_ptr<Thread>> threads;

	Process(size_t p_process_ID, char *p_working_directory);
	size_t Create_Thread(kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers);
	void Join_Thread(size_t thread_ID, uint16_t exit_code);
	uint16_t Kill_Thread(size_t thread_ID);
};
