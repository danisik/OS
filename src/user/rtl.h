#pragma once

#include "..\api\api.h"
#include <atomic>

namespace kiv_os_rtl {

	extern std::atomic<kiv_os::NOS_Error> Last_Error;

	//NOS_File_System
	bool Open_File(const char *buffer, const size_t buffer_size, kiv_os::THandle &file_handle, const bool exist, const kiv_os::NFile_Attributes attributes);
	bool Write_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &written);
	bool Read_File(const kiv_os::THandle file_handle, char* const buffer, const size_t buffer_size, size_t &read);
	bool Seek();
	bool Close_Handle(const kiv_os::THandle file_handle);
	bool Delete_File(const char* filename);
	bool Set_Working_Dir(const char *dir);
	bool Get_Working_Dir(const char *buffer, const size_t buffer_size, size_t &read);
	bool Create_Pipe(kiv_os::THandle handlers[]);

	//NOS_Process
	bool Clone(kiv_os::THandle& pid, const char* function, const char* arguments, kiv_os::THandle stdin_handle, kiv_os::THandle stdout_handle);
	bool Wait_For(kiv_os::THandle handle);
	std::uint8_t Read_Exit_Code(kiv_os::THandle handle);
	bool Exit(uint16_t exitcode);
	bool Shutdown();
	bool Register_Signal_Handler(kiv_os::NSignal_Id signal, kiv_os::TThread_Proc handler);

	kiv_os::THandle Create_Thread(void* function, void* data, kiv_os::THandle stdin_handle, kiv_os::THandle stdout_handle);
}