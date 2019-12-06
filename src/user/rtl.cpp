#include "rtl.h"

std::atomic<kiv_os::NOS_Error> kiv_os_rtl::Last_Error;

kiv_hal::TRegisters Prepare_SysCall_Context(kiv_os::NOS_Service_Major major, uint8_t minor) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(major);
	regs.rax.l = minor;
	return regs;
}

void kiv_os_rtl::Default_Signal_Handler() {
	// Do nothing.
	return;
}

//NOS_File_System
bool kiv_os_rtl::Open_File(const char *file_name, kiv_os::NOpen_File flags, kiv_os::NFile_Attributes attributes, kiv_os::THandle &open) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File));

	// Put it into string (dunno why this works with string and not only with const char*).
	std::string str = std::string(file_name, strlen(file_name));
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(str.data());	
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(flags);
	regs.rdi.r = static_cast<decltype(regs.rdi.r)>(attributes);

	bool syscall_result = kiv_os::Sys_Call(regs);
	open = regs.rax.x;
	return syscall_result;
}

bool kiv_os_rtl::Write_File(kiv_os::THandle file_handle, const char *buffer, size_t buffer_size, size_t &written) {
	
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Write_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;

	//TODO Write_File: check if file is read only (NICE TO HAVE).

	const bool syscall_result = kiv_os::Sys_Call(regs);
	written = regs.rax.r;
	return syscall_result;
}

bool kiv_os_rtl::Read_File(kiv_os::THandle file_handle, const char *buffer, size_t buffer_size, size_t &read) {
	kiv_hal::TRegisters regs =  Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Read_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;	

	const bool syscall_result = kiv_os::Sys_Call(regs);
	read = regs.rax.r;
	return syscall_result;
}

bool kiv_os_rtl::Seek(kiv_os::THandle file_handle, kiv_os::NFile_Seek seek_operation, kiv_os::NFile_Seek new_position, size_t &position) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Seek));
	
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = static_cast<decltype(regs.rdi.r)>(position);
	regs.rcx.x = static_cast<decltype(regs.rcx.x)>(seek_operation);
	
	if (kiv_os::NFile_Seek::Set_Position == seek_operation) {
		regs.rcx.x = static_cast<decltype(regs.rcx.x)>(new_position);
	}	

	bool syscall_result = kiv_os::Sys_Call(regs);

	if (kiv_os::NFile_Seek::Get_Position == seek_operation) {
		position = regs.rax.r;
	}

	return syscall_result;
}

bool kiv_os_rtl::Close_Handle(kiv_os::THandle file_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));

	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Delete_File(const char *file_name) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Delete_File));
	
	std::string str = std::string(file_name, strlen(file_name));
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(str.data());

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Set_Working_Dir(const char *new_directory, bool &success) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Set_Working_Dir));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(new_directory);

	bool syscall_result = kiv_os::Sys_Call(regs);
	success = static_cast<bool>(regs.rax.x);
	return syscall_result;
}

bool kiv_os_rtl::Get_Working_Dir(char *path, size_t path_size, size_t &written_chars) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Get_Working_Dir));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(path);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(path_size);

	bool syscall_result = kiv_os::Sys_Call(regs);
	written_chars = regs.rax.r;
	return syscall_result;
}

bool kiv_os_rtl::Create_Pipe(kiv_os::THandle *pipe_handles) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Create_Pipe));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(pipe_handles);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

//NOS_Process
bool kiv_os_rtl::Clone(kiv_os::NClone clone_type, void *export_name, void *arguments, kiv_os::THandle stdin_handle, kiv_os::THandle stdout_handle, kiv_os::THandle &process) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));

	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(clone_type);

	if (clone_type == kiv_os::NClone::Create_Process) {

		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(export_name);
		regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(arguments);
		regs.rbx.e = (stdin_handle << 16) | stdout_handle;

	}
	else {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(export_name);
		regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(arguments);
	}

	bool syscall_result = kiv_os::Sys_Call(regs);

	process = static_cast<kiv_os::THandle>(regs.rax.r);

	return syscall_result;
}

bool kiv_os_rtl::Wait_For(kiv_os::THandle process_handlers[], int process_handlers_count, kiv_os::THandle &signalized_handler) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(process_handlers);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(process_handlers_count);

	bool syscall_result = kiv_os::Sys_Call(regs);
	signalized_handler = static_cast<kiv_os::THandle>(regs.rax.x);
	return syscall_result;
}

bool kiv_os_rtl::Read_Exit_Code(kiv_os::THandle process_handle, uint16_t &exit_code) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(process_handle);

	bool syscall_result = kiv_os::Sys_Call(regs);
	exit_code = static_cast<size_t>(regs.rax.x);
	return syscall_result;
}

bool kiv_os_rtl::Exit(uint16_t exit_code) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Exit));

	regs.rcx.x = static_cast<decltype(regs.rcx.x)>(exit_code);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Shutdown() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Shutdown));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Register_Signal_Handler(kiv_os::NSignal_Id signal_Id, kiv_os::TThread_Proc process_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Register_Signal_Handler));

	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(signal_Id);

	if (process_handle == 0) {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(Default_Signal_Handler);
	}
	else {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(process_handle);
	}
	

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

