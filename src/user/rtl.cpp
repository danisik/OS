#include "rtl.h"

std::atomic<kiv_os::NOS_Error> kiv_os_rtl::Last_Error;

kiv_hal::TRegisters Prepare_SysCall_Context(kiv_os::NOS_Service_Major major, uint8_t minor) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(major);
	regs.rax.l = minor;
	return regs;
}

uint16_t str_to_uint16(const char *str) {
	char *end;
	errno = 0;
	long val = strtol(str, &end, 10);
	return (uint16_t)val;
}

//NOS_File_System
bool kiv_os_rtl::Open_File(const char *file_name, const kiv_os::NOpen_File flags, const kiv_os::NFile_Attributes attributes, kiv_os::THandle &open) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File));

	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(str_to_uint16(file_name));	
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(flags);
	regs.rdi.r = static_cast<decltype(regs.rdi.r)>(attributes);

	bool syscall_result = kiv_os::Sys_Call(regs);
	open = regs.rax.x;
	return syscall_result;
}

bool kiv_os_rtl::Write_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &written) {
	
	if (buffer_size == 0) {
		return true;
	}
	
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Write_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;

	//TODO Write_File: check if file is read only (NICE TO HAVE).

	const bool syscall_result = kiv_os::Sys_Call(regs);
	written = regs.rax.r;
	return syscall_result;
}

bool kiv_os_rtl::Read_File(const kiv_os::THandle file_handle, const char *buffer, const size_t buffer_size, size_t &read) {
	kiv_hal::TRegisters regs =  Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Read_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;	

	const bool syscall_result = kiv_os::Sys_Call(regs);
	read = regs.rax.r;
	return syscall_result;
}

bool kiv_os_rtl::Seek(const kiv_os::THandle file_handle, const kiv_os::NFile_Seek seek_operation, const kiv_os::NFile_Seek new_position, size_t &position) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Seek));
	
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = static_cast<decltype(regs.rdi.r)>(new_position);
	
	if (kiv_os::NFile_Seek::Set_Size == seek_operation) {
		// TODO Seek: Set size of file on this position (NEEDED).
	}	

	bool syscall_result = kiv_os::Sys_Call(regs);

	if (kiv_os::NFile_Seek::Get_Position == seek_operation) {
		position = regs.rax.r;
	}

	return syscall_result;
}

bool kiv_os_rtl::Close_Handle(const kiv_os::THandle file_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));

	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(file_handle);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Delete_File(const char *file_name) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Delete_File));
	
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(file_name);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Set_Working_Dir(const char *new_directory) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Set_Working_Dir));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(new_directory);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Get_Working_Dir(const char *path, const size_t path_size, size_t &written_chars) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Get_Working_Dir));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(path);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(path_size);

	bool syscall_result = kiv_os::Sys_Call(regs);
	written_chars = regs.rax.r;
	return syscall_result;
}

bool kiv_os_rtl::Create_Pipe(const kiv_os::THandle pipein_handle, const kiv_os::THandle pipeout_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Create_Pipe));

	kiv_os::THandle pipe_handle[2];
	pipe_handle[0] = pipein_handle;
	pipe_handle[1] = pipeout_handle;

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(pipe_handle);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

//NOS_Process
bool kiv_os_rtl::Clone(kiv_os::NClone clone_type, const char *export_name, const char *arguments, const kiv_os::THandle stdin_handle, const kiv_os::THandle stdout_handle, kiv_os::THandle &process) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));

	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(clone_type);

	// TODO - registry podle api.h -> pøidat podmínku na create process | create thread, a podle toho pøiøadit urèité registry.
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(export_name);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(arguments);
	regs.rbx.e = (stdin_handle << 16) | stdout_handle;

	bool syscall_result = kiv_os::Sys_Call(regs);

	process = static_cast<kiv_os::THandle>(regs.rax.r);

	return syscall_result;
}

bool kiv_os_rtl::Wait_For(const kiv_os::THandle process_handlers[]) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));

	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(process_handlers);
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(sizeof(process_handlers) / sizeof(kiv_os::THandle*));

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

std::uint8_t kiv_os_rtl::Read_Exit_Code(const kiv_os::THandle process_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	
	//.r register viz Exit method.
	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(process_handle);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Exit(const uint16_t exit_code) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Exit));

	//NOS_Error
	//Using .r register, because kiv_os::Sys_Call using .r register (uint64).
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(exit_code);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Shutdown() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Shutdown));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool kiv_os_rtl::Register_Signal_Handler(const kiv_os::NSignal_Id signal_Id, const kiv_os::TThread_Proc process_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Register_Signal_Handler));

	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(signal_Id);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(process_handle);

	/*
	TODO Register_Signal_Handler: ask about handler, when = 0 (NEEDED).
	if (handler == 0) {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(process_handle);
	}
	else {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(process_handle);
	}
	*/

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

