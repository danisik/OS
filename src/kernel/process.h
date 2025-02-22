#pragma once


#include "thread.h"
#include "Header.h"

#include <map>
#include <string>

class Process
{
	public:
		size_t process_ID;
		size_t process_thread_ID;
		State state;
		std::vector<char> name;
		std::vector<Mft_Item*> working_dir;
		std::map<size_t, std::unique_ptr<Thread>> threads;
		kiv_os::THandle handle_in;
		kiv_os::THandle handle_out;

		Process(size_t p_process_ID, const char *p_name, std::vector<Mft_Item*> p_working_dir, kiv_os::THandle p_handle_in, kiv_os::THandle p_handle_out);
		Process(size_t p_process_ID, const char *p_name);
		size_t Create_Thread(kiv_os::TThread_Proc entry_point, kiv_hal::TRegisters registers, const char* arguments);
		void Join_Thread(size_t thread_ID, uint16_t exit_code);
};
