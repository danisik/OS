#pragma once

#include "..\api\api.h"
#include <atomic>

namespace kiv_os_rtl {
	extern std::atomic<kiv_os::NOS_Error> Last_Error;

	//NOS_File_System
	bool Open_File(const char *fileName, const kiv_os::NOpen_File flags, const kiv_os::NFile_Attributes attributes, kiv_os::THandle &open);
	bool Write_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &written);
	bool Read_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &read);
	bool Seek(const kiv_os::THandle file_handle, const kiv_os::NFile_Seek seek_operation, const kiv_os::NFile_Seek new_position, size_t &position);
	bool Close_Handle(const kiv_os::THandle file_handle);
	bool Delete_File(const char *fileName);
	bool Set_Working_Dir(const char *new_directory);
	bool Get_Working_Dir(const char *path, const size_t path_size, size_t &written_chars);
	bool Create_Pipe(const kiv_os::THandle pipein_handle, const kiv_os::THandle pipeout_handle);

	//NOS_Process
	bool Clone(const char *export_name, const char *arguments, const kiv_os::THandle stdin_handle, const kiv_os::THandle stdout_handle, kiv_os::THandle &process);
	bool Wait_For(const kiv_os::THandle process_handle);
	std::uint8_t Read_Exit_Code(const kiv_os::THandle process_handle);
	bool Exit(const uint16_t exitcode);
	bool Shutdown();
	bool Register_Signal_Handler(const kiv_os::NSignal_Id signal_Id, const kiv_os::TThread_Proc process_handle);

}