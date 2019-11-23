#pragma once

#include "io.h"

std::mutex working_directory_mutex;
std::mutex io_mutex;
std::mutex pipe_mutex;

IO::IO(IO_Process *i_io_process, VFS *i_vfs) {
	io_process = i_io_process;
	vfs = i_vfs;
}

void IO::Open_File(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);
	std::lock_guard<std::mutex> lock_mutex_process(io_process->io_process_mutex);

	char* file_name = reinterpret_cast<char*>(regs.rdx.r);
	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(regs.rcx.r);
	kiv_os::NFile_Attributes attributes = static_cast<kiv_os::NFile_Attributes>(regs.rdi.r);
	HANDLE handle = NULL;

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	
	Exist_Item* item = Functions::Check_Path(vfs, file_name, io_process->processes[current_process_ID]->working_dir);

	if (item->path_exists && item->exists && item->is_directory) {
		// TODO Open_File: Open existing directory.
	}
	else if (item->path_exists && !item->exists) {
		// TODO Open_File: Set handler.
		Commands::Create_Directory(vfs, file_name, io_process->processes[current_process_ID]->working_dir);
	}
	else if (!item->path_exists) {
		// Path did not exists. 
		return;
	}
	

	// TODO Open_File: functional code.

	/*
	Nasledujici dve vetve jsou ukazka, ze starsiho zadani, ktere ukazuji, jak mate mapovat 
	Windows HANDLE na kiv_os handle a zpet, vcetne jejich alokace a uvolneni

	HANDLE result = CreateFileA((char*)regs.rdx.r, GENERIC_READ | GENERIC_WRITE, (DWORD)regs.rcx.r, 0, OPEN_EXISTING, 0, 0);
			//zde je treba podle Rxc doresit shared_read, shared_write, OPEN_EXISING, etc. podle potreby
			regs.flags.carry = result == INVALID_HANDLE_VALUE;
			if (!regs.flags.carry) regs.rax.x = Convert_Native_Handle(result);
			else regs.rax.r = GetLastError();
	*/

	// Return handler.
	regs.rax.x = Convert_Native_Handle(static_cast<HANDLE>(handle));
}

void IO::Write_File(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	auto file_handle = static_cast<IO_Handle*>(Resolve_kiv_os_Handle(regs.rdx.x));
	char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	size_t buffer_length = regs.rcx.r;	

	regs.rax.r = file_handle->Write(buffer, buffer_length);
}

void IO::Print_VFS() {
	printf("\n");
	Functions::Print_MFT(vfs);
	Functions::Print_Bitmap(vfs);
}

void IO::Read_File(kiv_hal::TRegisters &regs) {
	auto file_handle = static_cast<IO_Handle*>(Resolve_kiv_os_Handle(regs.rdx.x));
	char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	size_t buffer_length = regs.rcx.r;

	regs.rax.r = file_handle->Read(buffer, buffer_length);
}

void IO::Seek(kiv_hal::TRegisters &regs) {
	//std::lock_guard<std::mutex> lock_mutex(io_mutex);

	auto file_handle = Resolve_kiv_os_Handle(regs.rdx.x);
	kiv_os::NFile_Seek new_position = static_cast<kiv_os::NFile_Seek>(regs.rdi.r);
	kiv_os::THandle position = NULL;

	// TODO Seek: functional code.

	regs.rax.x = position;
}

void IO::Close_Handle(kiv_hal::TRegisters &regs) {
	Remove_Handle(regs.rdx.x);
}

void IO::Delete_File(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);
	std::lock_guard<std::mutex> lock_mutex_process(io_process->io_process_mutex);

	char *file_name = reinterpret_cast<char*>(regs.rdx.r);

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	Commands::Remove_Directory(vfs, file_name, io_process->processes[current_process_ID]->working_dir);

	// TODO Delete_File: functional code.
}

void IO::Set_Working_Dir(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(working_directory_mutex);
	char *new_directory = reinterpret_cast<char*>(regs.rdx.r);

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	bool success = Commands::Move_To_Directory(vfs, new_directory, io_process->processes[current_process_ID]->working_dir);

	regs.rax.x = static_cast<decltype(regs.rax.x)>(success);
}

void IO::Get_Working_Dir(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(working_directory_mutex);
	char *path = reinterpret_cast<char*>(regs.rdx.r);
	size_t path_size = static_cast<size_t>(regs.rcx.r);
	size_t written_chars = 0;

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	std::vector<Mft_Item*> current_path = io_process->processes[current_process_ID]->working_dir;

	std::string path_string;

	for (int i = 0; i < current_path.size(); i++) {
		path_string += io_process->processes[current_process_ID]->working_dir[i]->item_name;
		if (i > 0 && i < current_path.size() - 1) {
			path_string += '\\';
			written_chars++;
		}
		written_chars += strlen(current_path[i]->item_name);
	}

	strcpy_s(path, path_size, path_string.c_str());

	regs.rax.r = written_chars;
}

void IO::Create_Pipe(kiv_hal::TRegisters &regs) {
	//std::lock_guard<std::mutex> lock_mutex(pipe_mutex);

	auto pipe_handle = Resolve_kiv_os_Handle(regs.rdx.x);
	//pipe_handle[0] = pipein_handle;
	//pipe_handle[1] = pipeout_handle;

	// TODO Create_Pipe: functional code.
}


void IO::Handle_IO(kiv_hal::TRegisters &regs) {

	switch (static_cast<kiv_os::NOS_File_System>(regs.rax.l)) {

		case kiv_os::NOS_File_System::Open_File:
			Open_File(regs);		
			break;

		case kiv_os::NOS_File_System::Write_File:
			Write_File(regs);		
			break;

		case kiv_os::NOS_File_System::Read_File:
			Read_File(regs);
			break;

		case kiv_os::NOS_File_System::Seek:
			Seek(regs);
			break;

		case kiv_os::NOS_File_System::Close_Handle:
			Close_Handle(regs);
			break;

		case kiv_os::NOS_File_System::Delete_File:
			Delete_File(regs);
			break;

		case kiv_os::NOS_File_System::Set_Working_Dir:
			Set_Working_Dir(regs);
			break;

		case kiv_os::NOS_File_System::Get_Working_Dir:
			Get_Working_Dir(regs);
			break;

		case kiv_os::NOS_File_System::Create_Pipe:
			Create_Pipe(regs);
			break;
	}
}