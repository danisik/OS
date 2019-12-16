#pragma once

#include "io.h"

std::mutex working_directory_mutex;
std::mutex io_mutex;

IO::IO(uint64_t cluster_count, uint16_t cluster_size, int v_drive_i)
{
	io_process = std::make_unique<IO_Process>();
	io_process->shutdown_signalized = false;
	vfs = std::make_unique<VFS>(cluster_count, cluster_size, v_drive_i);

	vfs->Init_VFS(vfs);
}

void IO::Print_To_Console(const char* message)
{
	std::string str = std::string(message);
	kiv_hal::TRegisters registers;

	registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_String);
	registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(message);
	registers.rcx.r = str.length();
	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
}

void IO::Open_File(kiv_hal::TRegisters &regs)
{
	std::lock_guard<std::mutex> lock_mutex(io_mutex);
	std::lock_guard<std::mutex> lock_mutex_process(io_process->io_process_mutex);

	char* file_name = reinterpret_cast<char*>(regs.rdx.r);

	if (strcmp(file_name, "procfs") == 0)
	{
		Procfs_Handle *handle = new Procfs_Handle();
		regs.rax.x = Convert_Native_Handle(static_cast<Procfs_Handle*>(handle));
		return;
	}

	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(regs.rcx.r);
	kiv_os::NFile_Attributes attributes = static_cast<kiv_os::NFile_Attributes>(regs.rdi.r);

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;
	
	Exist_Item* item = Functions::Check_Path(vfs, file_name, io_process->processes[current_process_ID]->working_dir);

	if (!item->path_exists) 
	{
		Print_To_Console("Path did not exists.\n");
		regs.rax.x = -1;
		return;
	}

	if (flags == kiv_os::NOpen_File::fmOpen_Always)
	{

		if (!item->exists) 
		{
			Print_To_Console("File did not exists.\n");
			regs.rax.x = -1;
			return;
		}

		if (attributes == kiv_os::NFile_Attributes::Directory && item->is_directory == kiv_os::NFile_Attributes::Directory)
		{
			Directory_Handle *dir_handle = new Directory_Handle();
			dir_handle->item = Functions::Get_Mft_Item(vfs, item->uid);
			regs.rax.x = Convert_Native_Handle(static_cast<Item_Handle*>(dir_handle));
		}
		else if (attributes == kiv_os::NFile_Attributes::System_File && item->is_directory == kiv_os::NFile_Attributes::System_File)
		{
			File_Handle *file_handle = new File_Handle();
			file_handle->item = Functions::Get_Mft_Item(vfs, item->uid);
			regs.rax.x = Convert_Native_Handle(static_cast<Item_Handle*>(file_handle));
		}
		else if (attributes == kiv_os::NFile_Attributes::Directory && item->is_directory == kiv_os::NFile_Attributes::System_File) 
		{
			Print_To_Console("Expected directory but item is file.\n");
			regs.rax.x = -1;
			return;
		}
		else 
		{
			Print_To_Console("Expected file but item is directory.\n");
			regs.rax.x = -1;
			return;
		}
	}
	else 
	{

		if (item->exists && attributes == kiv_os::NFile_Attributes::System_File)
		{
			File_Handle *file_handle = new File_Handle();
			file_handle->item = Functions::Get_Mft_Item(vfs, item->uid);
			regs.rax.x = Convert_Native_Handle(static_cast<Item_Handle*>(file_handle));
			return;
		}

		size_t item_uid = Functions::Create_Item(vfs, file_name, io_process->processes[current_process_ID]->working_dir, attributes);

		if (item_uid == (size_t)(-1)) 
		{
			Print_To_Console("Path not found.\n");
		}

		if (item_uid == (size_t)(-2))
		{
			Print_To_Console("Item already exists.\n");
		}

		if (attributes == kiv_os::NFile_Attributes::Directory)
		{
			Directory_Handle *dir_handle = new Directory_Handle();
			dir_handle->item = Functions::Get_Mft_Item(vfs, item_uid);
			regs.rax.x = Convert_Native_Handle(static_cast<Item_Handle*>(dir_handle));
		}
		else 
		{
			File_Handle *file_handle = new File_Handle();
			file_handle->item = Functions::Get_Mft_Item(vfs, item_uid);
			regs.rax.x = Convert_Native_Handle(static_cast<Item_Handle*>(file_handle));
		}
	}

	return;
}

void IO::Write_File(kiv_hal::TRegisters &regs) 
{
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	auto file_handle = static_cast<IO_Handle*>(Resolve_kiv_os_Handle(regs.rdx.x));
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	size_t buffer_length = regs.rcx.r;	

	regs.rax.r = file_handle->Write(buffer, buffer_length, vfs, io_process);
}

void IO::Read_File(kiv_hal::TRegisters &regs) 
{
	auto file_handle = static_cast<IO_Handle*>(Resolve_kiv_os_Handle(regs.rdx.x));
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	size_t buffer_length = regs.rcx.r;

	regs.rax.r = file_handle->Read(buffer, buffer_length, vfs, io_process);
}

void IO::Seek(kiv_hal::TRegisters &regs) 
{
	auto file_handle = static_cast<Item_Handle*>(Resolve_kiv_os_Handle(regs.rdx.x));
	if (file_handle == INVALID_HANDLE_VALUE) 
	{
		return;
	}

	kiv_os::NFile_Seek new_position = static_cast<kiv_os::NFile_Seek>(regs.rcx.x);
	size_t position = static_cast<size_t>(regs.rdi.r);
	Mft_Item *item = file_handle->item;
	size_t item_size = item->item_size;

	if (new_position == kiv_os::NFile_Seek::Set_Size) 
	{
		if (position == 0)
		{
			position = 1;
		}
		item->item_size = position;
		Functions::Remove_From_Data_Block(vfs, item);
		Functions::Write_To_Data_Block(vfs, item);
		Functions::Save_VFS_MFT(vfs);
		Functions::Save_VFS_MFT_Item(vfs, item->uid);
		return;
	}
	else if (new_position == kiv_os::NFile_Seek::Get_Position)
	{
		regs.rax.r = file_handle->seek;
		return;
	}
	else 
	{
		file_handle->Seek(new_position, position, item->item_size);
		return;
	}
}

void IO::Close_Handle(kiv_hal::TRegisters &regs) 
{
	auto handle = static_cast<IO_Handle*>(Resolve_kiv_os_Handle(regs.rdx.x));
	if (handle != INVALID_HANDLE_VALUE)
	{
		handle->Close();
		Remove_Handle(regs.rdx.x);
	}
}

void IO::Delete_File(kiv_hal::TRegisters &regs)
{
	std::lock_guard<std::mutex> lock_mutex(io_mutex);
	std::lock_guard<std::mutex> lock_mutex_process(io_process->io_process_mutex);

	char *file_name = reinterpret_cast<char*>(regs.rdx.r);

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	Functions::Remove_Item(vfs, file_name, io_process->processes[current_process_ID]->working_dir);
}

void IO::Set_Working_Dir(kiv_hal::TRegisters &regs) 
{
	std::lock_guard<std::mutex> lock_mutex(working_directory_mutex);
	char *new_directory = reinterpret_cast<char*>(regs.rdx.r);

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	bool success = Functions::Move_To_Directory(vfs, new_directory, io_process->processes[current_process_ID]->working_dir);

	regs.rax.x = static_cast<decltype(regs.rax.x)>(success);
}

void IO::Get_Working_Dir(kiv_hal::TRegisters &regs) 
{
	std::lock_guard<std::mutex> lock_mutex(working_directory_mutex);
	char *path = reinterpret_cast<char*>(regs.rdx.r);
	size_t path_size = static_cast<size_t>(regs.rcx.r);
	size_t written_chars = 0;

	size_t current_thread_ID = Thread::Get_Thread_ID(std::this_thread::get_id());
	size_t current_process_ID = io_process->thread_ID_to_process_ID.find(current_thread_ID)->second;

	std::vector<Mft_Item*> current_path = io_process->processes[current_process_ID]->working_dir;

	std::string path_string;

	for (int i = 0; i < current_path.size(); i++)
	{
		path_string += io_process->processes[current_process_ID]->working_dir[i]->item_name;
		if (i < current_path.size() - 1)
		{
			path_string += '\\';
			written_chars++;
		}
		written_chars += strlen(current_path[i]->item_name);
	}

	strcpy_s(path, path_size, path_string.c_str());

	regs.rax.r = written_chars;
}

void IO::Create_Pipe(kiv_hal::TRegisters &regs)
{
	Pipe_Handle *pipein_handle = new Pipe_Handle(PIPE_SIZE);

	Pipe_Handle *pipeout_handle = new Pipe_Handle(pipein_handle->pipe);

	kiv_os::THandle *pipe_handle = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);
	*(pipe_handle) = Convert_Native_Handle(static_cast<HANDLE>(pipein_handle));
	*(pipe_handle + 1) = Convert_Native_Handle(static_cast<HANDLE>(pipeout_handle));
}


void IO::Handle_IO(kiv_hal::TRegisters &regs)
{

	switch (static_cast<kiv_os::NOS_File_System>(regs.rax.l))
	{

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