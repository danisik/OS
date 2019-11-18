#pragma once

#include "../api/api.h"
#include "process.h"
#include "handles.h"
#include "globals.h"

#include <map>
#include <mutex>

#define MIN_THREAD_THANDLE 10000

class IO_Process {

	public:
		size_t first_free_process_ID = 0;
		kiv_os::THandle first_free_thread_ID = MIN_THREAD_THANDLE;

		std::mutex io_process_mutex;
		std::mutex shutdown_mutex;

		// Index -> process_ID; Value -> Process.
		std::map<size_t, std::unique_ptr<Process>> processes;
		// Index -> thread_ID; Value -> process_ID.
		std::map<size_t, size_t> thread_ID_to_process_ID;
		// Index -> handle of thread / process; Value -> thread_ID.
		std::map<kiv_os::THandle, size_t> t_handle_to_thread_ID;

		size_t Get_Free_Process_ID(); // Tested.
		void Set_Free_Process_ID(size_t process_ID);

		kiv_os::THandle Get_Free_Thread_ID();
		void Set_Free_Thread_ID(kiv_os::THandle thread_ID);
		kiv_os::THandle Find_Free_Thread_ID(size_t thread_ID);

		kiv_os::THandle Get_THandle_From_Thread_ID(size_t thread_ID);

		void Clone(kiv_hal::TRegisters &regs); // Tested.
		void Clone_Process(kiv_hal::TRegisters &regs); // Tested.
		void Clone_Thread(kiv_hal::TRegisters &regs); // Tested.

		void Wait_For(kiv_hal::TRegisters &regs);  // Tested.
		void Read_Exit_Code(kiv_hal::TRegisters &regs); // Tested.
		void Exit(kiv_hal::TRegisters &regs); // Tested.
		void Shutdown(kiv_hal::TRegisters &regs); // Test it after Exit and Read_Exit_Code are writed.
		void Register_Signal_Handler(kiv_hal::TRegisters &regs); // Tested.

		void Handle_Process(kiv_hal::TRegisters &regs); // Tested.

		void Notify(size_t sleeped_thread_ID, size_t waiting_thread_ID);
		void Notify_All(size_t thread_ID);

		void Clear_Processes();
};