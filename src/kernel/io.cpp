#pragma once

#include "io.h"


std::mutex io_mutex;
std::mutex pipe_mutex;

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

void Open_File(kiv_hal::TRegisters &regs) {
	//std::lock_guard<std::mutex> lock_mutex(io_mutex);

	char* fileName = reinterpret_cast<char*>(regs.rdx.x);
	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(regs.rcx.r);
	kiv_os::NFile_Attributes attributes = static_cast<kiv_os::NFile_Attributes>(regs.rdi.r);
	HANDLE handle = NULL;

	// TODO Open_File: functional code.

	/*			HANDLE result = CreateFileA((char*)regs.rdx.r, GENERIC_READ | GENERIC_WRITE, (DWORD)regs.rcx.r, 0, OPEN_EXISTING, 0, 0);
			//zde je treba podle Rxc doresit shared_read, shared_write, OPEN_EXISING, etc. podle potreby
			regs.flags.carry = result == INVALID_HANDLE_VALUE;
			if (!regs.flags.carry) regs.rax.x = Convert_Native_Handle(result);
			else regs.rax.r = GetLastError();
	*/

	regs.rax.x = Convert_Native_Handle(static_cast<HANDLE>(handle));
}

void Write_File(kiv_hal::TRegisters &regs) {
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

void Read_File(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	//HANDLE file_handle = Resolve_kiv_os_Handle(regs.rdx.x);
	//char *buffer = reinterpret_cast<char*>(regs.rdi.r);
	//size_t buffer_size = regs.rcx.r;
	//size_t read = NULL;

	// TODO Read_File: functional code.

	//regs.rax.x = read;



	regs.rax.r = Read_Line_From_Console(reinterpret_cast<char*>(regs.rdi.r), regs.rcx.r);	
}

void Seek(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	kiv_os::THandle file_handle = static_cast<kiv_os::THandle>(regs.rdx.x);
	kiv_os::NFile_Seek new_position = static_cast<kiv_os::NFile_Seek>(regs.rdi.r);
	kiv_os::THandle position = NULL;

	// TODO Seek: functional code.

	regs.rax.x = position;
}

void Close_Handle(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	HANDLE file_handle = Resolve_kiv_os_Handle(regs.rdx.x);

	regs.flags.carry = !CloseHandle(file_handle);
	if (!regs.flags.carry) {
		Remove_Handle(regs.rdx.x);
	}
	else {
		regs.rax.r = GetLastError();
	}
}

void Delete_File(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	char *fileName = reinterpret_cast<char*>(regs.rdx.r);

	// TODO Delete_File: functional code.
}

void Set_Working_Dir(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	char *new_directory = reinterpret_cast<char*>(regs.rdx.r);
	
	// TODO Set_Working_Dir: functional code.
}

void Get_Working_Dir(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(io_mutex);

	char *path = reinterpret_cast<char*>(regs.rdx.r);
	size_t path_size = static_cast<size_t>(regs.rcx.r);
	size_t written_chars = NULL;

	// TODO Get_Working_Dir: functional code.

	regs.rax.r = written_chars;
}

void Create_Pipe(kiv_hal::TRegisters &regs) {
	std::lock_guard<std::mutex> lock_mutex(pipe_mutex);

	kiv_os::THandle *pipe_handle = reinterpret_cast<kiv_os::THandle*>(regs.rdx.r);
	//pipe_handle[0] = pipein_handle;
	//pipe_handle[1] = pipeout_handle;

	// TODO Create_Pipe: functional code.
}


void Handle_IO(kiv_hal::TRegisters &regs) {

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