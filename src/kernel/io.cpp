#pragma once

#include "io.h"

std::mutex working_directory_mutex;
std::mutex io_mutex;
std::mutex pipe_mutex;

IO::IO(IO_Process *i_io_process, VFS *i_vfs) {
	io_process = i_io_process;
	vfs = i_vfs;
}

size_t Read_Line_From_Console(char *buffer, const size_t buffer_size) {
	kiv_hal::TRegisters registers;

	size_t pos = 0;
	while (pos < buffer_size) {
		//read char
		registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NKeyboard::Read_Char);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Keyboard, registers);

		if (!registers.flags.non_zero) break;	//nic jsme neprecetli, 
												//pokud je rax.l EOT, pak byl zrejme vstup korektne ukoncen
												//jinak zrejme doslo k chybe zarizeni

		char ch = registers.rax.l;

		//osetrime zname kody
		switch (static_cast<kiv_hal::NControl_Codes>(ch)) {
		case kiv_hal::NControl_Codes::BS: {
			//mazeme znak z bufferu
			if (pos > 0) pos--;

			registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_Control_Char);
			registers.rdx.l = ch;
			kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
		}
										  break;

		case kiv_hal::NControl_Codes::LF:  break;	//jenom pohltime, ale necteme
		case kiv_hal::NControl_Codes::NUL:			//chyba cteni?
		case kiv_hal::NControl_Codes::CR:  return pos;	//docetli jsme az po Enter


		default: buffer[pos] = ch;
			pos++;
			registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_String);
			registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(&ch);
			registers.rcx.r = 1;
			kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
			break;
		}
	}

	return pos;

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
		// TODO Open_File: Create a directory.
		Commands::Create_Directory(vfs, file_name, io_process->processes[current_process_ID]->working_dir);
	}
	else if (!item->path_exists) {
		// TODO Open_File: Path did not exists. 
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

	regs.rax.x = Convert_Native_Handle(static_cast<HANDLE>(handle));
}

void IO::Write_File(kiv_hal::TRegisters &regs) {
	//Spravne bychom nyni meli pouzit interni struktury kernelu a 
	//zadany handle resolvovat na konkretni objekt, ktery pise na konkretni zarizeni/souboru/roury.
	//Ale protoze je tohle jenom kostra, tak to rovnou biosem posleme na konzoli.

	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	HANDLE file_handle = Resolve_kiv_os_Handle(regs.rdx.x);
	char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	size_t buffer_size = regs.rcx.r;
	size_t write = NULL;

	// TODO Write_File: functional code.

	//Set_Error()
	regs.rax.r = write;

	kiv_hal::TRegisters registers;
	registers.rax.h = static_cast<decltype(registers.rax.h)>(kiv_hal::NVGA_BIOS::Write_String);
	registers.rdx.r = regs.rdi.r;
	registers.rcx = regs.rcx;
	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
	regs.flags.carry |= (registers.rax.r == 0 ? 1 : 0);	//jestli jsme nezapsali zadny znak, tak jiste doslo k nejake chybe
	regs.rax = registers.rcx;	//VGA BIOS nevraci pocet zapsanych znaku, tak predpokladame, ze zapsal vsechny

	//IN : dx je handle souboru, rdi je pointer na buffer, rcx je pocet bytu v bufferu k zapsani
	//OUT : rax je pocet zapsanych bytu

	//regs.rax.r = size_t
}

void IO::Print_VFS() {
	printf("\n");
	Functions::Print_MFT(vfs);
	Functions::Print_Bitmap(vfs);
}

void IO::Read_File(kiv_hal::TRegisters &regs) {
	//std::lock_guard<std::mutex> lock_mutex(io_mutex);

	//HANDLE file_handle = Resolve_kiv_os_Handle(regs.rdx.x);
	//char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	//size_t buffer_size = regs.rcx.r;
	//size_t read = NULL;

	// TODO Read_File: functional code.

	//regs.rax.x = read;

	regs.rax.r = Read_Line_From_Console(reinterpret_cast<char*>(regs.rdi.r), regs.rcx.r);	
}

void IO::Seek(kiv_hal::TRegisters &regs) {
	//std::lock_guard<std::mutex> lock_mutex(io_mutex);

	kiv_os::THandle file_handle = static_cast<kiv_os::THandle>(regs.rdx.x);
	kiv_os::NFile_Seek new_position = static_cast<kiv_os::NFile_Seek>(regs.rdi.r);
	kiv_os::THandle position = NULL;

	// TODO Seek: functional code.

	regs.rax.x = position;
}

void IO::Close_Handle(kiv_hal::TRegisters &regs) {
	//std::lock_guard<std::mutex> lock_mutex(io_mutex);

	HANDLE file_handle = Resolve_kiv_os_Handle(regs.rdx.x);

	regs.flags.carry = !CloseHandle(file_handle);
	if (!regs.flags.carry) {
		Remove_Handle(regs.rdx.x);
	}
	else {
		regs.rax.r = GetLastError();
	}
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

	kiv_os::THandle *pipe_handle = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);
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