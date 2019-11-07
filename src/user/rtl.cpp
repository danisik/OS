#include "rtl.h"

std::atomic<kiv_os::NOS_Error> kiv_os_rtl::Last_Error;

kiv_hal::TRegisters Prepare_SysCall_Context(kiv_os::NOS_Service_Major major, uint8_t minor) {
	kiv_hal::TRegisters regs;
	regs.rax.h = static_cast<uint8_t>(major);
	regs.rax.l = minor;
	return regs;
}

//NOS_File_System
// TODO: create open_file
bool Open_File(const char *buffer, const size_t buffer_size, kiv_os::THandle &file_handle, const bool exist, const kiv_os::NFile_Attributes attributes) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Open_File));
	bool syscall_result = kiv_os::Sys_Call(regs);
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

	const bool result = kiv_os::Sys_Call(regs);
	written = regs.rax.r;
	return result;
}

bool kiv_os_rtl::Read_File(const kiv_os::THandle file_handle, char* const buffer, const size_t buffer_size, size_t &read) {
	kiv_hal::TRegisters regs =  Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Read_File));
	regs.rdx.x = static_cast<decltype(regs.rdx.x)>(file_handle);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(buffer);
	regs.rcx.r = buffer_size;	

	const bool result = kiv_os::Sys_Call(regs);
	read = regs.rax.r;
	return result;
}

// TODO: create seek
bool Seek() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Seek));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

// TODO: create close_handle
bool Close_Handle(const kiv_os::THandle file_handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Close_Handle));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

// TODO: create delete_file
bool Delete_File(const char* filename) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Delete_File));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

// TODO: create set_working_dir
bool Set_Working_Dir(const char *dir) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Set_Working_Dir));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

// TODO: create get_working_dir
bool Get_Working_Dir(const char *buffer, const size_t buffer_size, size_t &read) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Get_Working_Dir));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

// TODO: create create_pipe
bool Create_Pipe(kiv_os::THandle handlers[]) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::File_System, static_cast<uint8_t>(kiv_os::NOS_File_System::Create_Pipe));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

//NOS_Process
bool Clone(const char* export_name, const char* arguments, kiv_os::THandle stdin_handle, kiv_os::THandle stdout_handle, kiv_os::THandle& process) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Clone));

	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(kiv_os::NClone::Create_Process);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(export_name);
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(arguments);
	regs.rbx.e = (stdin_handle << 16) | stdout_handle;

	bool syscall_result = kiv_os::Sys_Call(regs);

	process = static_cast<kiv_os::THandle>(regs.rax.r);

	return syscall_result;
}

bool Wait_For(kiv_os::THandle handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Wait_For));

	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(handle);
	//We are waiting for this one handle, so rcx is = 1.
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(1);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

std::uint8_t Read_Exit_Code(kiv_os::THandle handle) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Read_Exit_Code));
	
	//.r register viz Exit method.
	regs.rdx.r = static_cast<decltype(regs.rdx.r)>(handle);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool Exit(uint16_t exitcode) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Exit));

	//NOS_Error
	//Using .r register, because kiv_os::Sys_Call using .r register (uint64).
	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(exitcode);

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool Shutdown() {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Shutdown));
	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}

bool Register_Signal_Handler(kiv_os::NSignal_Id signal_Id, kiv_os::TThread_Proc handler) {
	kiv_hal::TRegisters regs = Prepare_SysCall_Context(kiv_os::NOS_Service_Major::Process, static_cast<uint8_t>(kiv_os::NOS_Process::Register_Signal_Handler));

	regs.rcx.r = static_cast<decltype(regs.rcx.r)>(signal_Id);
	regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(handler);

	/*
	if (handler == 0) {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(handler);
	}
	else {
		regs.rdx.r = reinterpret_cast<decltype(regs.rdx.r)>(handler);
	}
	*/

	bool syscall_result = kiv_os::Sys_Call(regs);
	return syscall_result;
}


