#pragma once

#include "..\api\api.h"
#include <atomic>
#include <vector>

enum class Exit_Codes {
	EXIT_CODE_SUCCESS = 0,
	EXIT_CODE_FAILURE,
	EXIT_CODE_FATAL
};

namespace kiv_os_rtl {
	extern std::atomic<kiv_os::NOS_Error> Last_Error;

	void Default_Signal_Handler();

	//NOS_File_System
	bool Open_File(const char *file_name, kiv_os::NOpen_File flags, kiv_os::NFile_Attributes attributes, kiv_os::THandle &open);
	bool Write_File(kiv_os::THandle file_handle, const char *buffer, size_t buffer_size, size_t &written);
	bool Read_File(kiv_os::THandle file_handle, const char *buffer, size_t buffer_size, size_t &read);
	bool Seek(kiv_os::THandle file_handle, kiv_os::NFile_Seek seek_operation, kiv_os::NFile_Seek new_position, size_t &position);
	bool Close_Handle(kiv_os::THandle file_handle);
	bool Delete_File(const char *file_name);
	bool Set_Working_Dir(const char *new_directory, bool &success);
	bool Get_Working_Dir(char *path, size_t path_size, size_t &written_chars);
	bool Create_Pipe(kiv_os::THandle *pipe_handles);

	//NOS_Process
	bool Clone(kiv_os::NClone clone_type, void *export_name, void *arguments, kiv_os::THandle stdin_handle, kiv_os::THandle stdout_handle, kiv_os::THandle &process);
	bool Wait_For(kiv_os::THandle process_handlers[], int process_handlers_count, kiv_os::THandle &signalized_handler);
	bool Read_Exit_Code(kiv_os::THandle process_handle, uint16_t &exit_code);
	bool Exit(uint16_t exit_code);
	bool Shutdown();
	bool Register_Signal_Handler(kiv_os::NSignal_Id signal_Id, kiv_os::TThread_Proc process_handle);

}