#pragma once

#include "../api/api.h"
#include "process.h"
#include "handles.h"
#include "user_programs.h"

#include <map>
#include <mutex>

class IO_Process {

	public:
		size_t first_free_process_ID = 0;

		std::mutex io_process_mutex;

		// Index -> process_ID; Value -> Process.
		std::map<size_t, std::unique_ptr<Process>> processes;
		// Index -> thread_ID; Value -> process_ID
		std::map<size_t, size_t> thread_ID_to_process_ID;

		size_t Get_Free_Process_ID();

		void Clone(kiv_hal::TRegisters &regs);
		void Clone_Process(kiv_hal::TRegisters &regs);
		void Clone_Thread(kiv_hal::TRegisters &regs);

		void Wait_For(kiv_hal::TRegisters &regs);
		void Read_Exit_Code(kiv_hal::TRegisters &regs);
		void Exit(kiv_hal::TRegisters &regs);
		void Shutdown(kiv_hal::TRegisters &regs);
		void Register_Signal_Handler(kiv_hal::TRegisters &regs);

		void Handle_Process(kiv_hal::TRegisters &regs);

};