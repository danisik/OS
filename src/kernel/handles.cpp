#include "handles.h"

#include <map>
#include <mutex>
#include <random>

std::map<kiv_os::THandle, HANDLE> Handles;
std::mutex Handles_Guard;
kiv_os::THandle Last_Handle = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 6);


kiv_os::THandle Convert_Native_Handle(const HANDLE hnd) {
	std::lock_guard<std::mutex> guard(Handles_Guard);

	Last_Handle += dis(gen);	//vygenerujeme novy interni handle s nahodnou hodnotou

	Handles.insert(std::pair<kiv_os::THandle, HANDLE>(Last_Handle, hnd));

	return Last_Handle;
}

HANDLE Resolve_kiv_os_Handle(const kiv_os::THandle hnd) {
	std::lock_guard<std::mutex> guard(Handles_Guard);

	auto resolved = Handles.find(hnd);
	if (resolved != Handles.end()) {
		return resolved->second;
	}
	else
		return INVALID_HANDLE_VALUE;
}

bool Remove_Handle(const kiv_os::THandle hnd) {
	std::lock_guard<std::mutex> guard(Handles_Guard);

	return Handles.erase(hnd) == 1;
}

//------------------------
//-----Handle methods-----
//------------------------

size_t IO_Handle::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	// Do nothing.
	return 0;
}

size_t IO_Handle::Write(char *buffer, size_t buffer_length, VFS *vfs) {
	// Do nothing.
	return 0;
}

void IO_Handle::Close() {
	// Do nothing.
	return;
}

size_t IO_Handle::Seek(kiv_os::NFile_Seek new_position, size_t position, size_t size) {

	switch (new_position) {
		case kiv_os::NFile_Seek::Beginning:
			seek = position;
			if (seek > size) {
				seek = size;
			}
			if (seek < 1) {
				seek = 1;
			}
			break;
		case kiv_os::NFile_Seek::Current:
			seek += position;
			if (seek > size) {
				seek = size;
			}
			if (seek < 1) {
				seek == 1;
			}
			break;
		case kiv_os::NFile_Seek::End:
			seek = size - position;
			if (seek < 1) {
				seek = 1;
			}
			break;
	}
	printf("Seek position: %zd\n", seek);
	return seek;
}

//------------------------
//---STD handle methods---
//------------------------

size_t STD_Handle_In::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	kiv_hal::TRegisters registers;

	size_t pos = 0;
	while (pos < buffer_length) {
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

size_t STD_Handle_Out::Write(char *buffer, size_t buffer_length, VFS *vfs) {
	kiv_hal::TRegisters registers;
	registers.rax.h = static_cast<decltype(registers.rax.h)>(kiv_hal::NVGA_BIOS::Write_String);
	registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(buffer);
	registers.rcx.r = buffer_length;
	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);

	return registers.rcx.r;
}

//------------------------
//---File handle methods--
//------------------------

size_t File_Handle::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	return 0;
}

size_t File_Handle::Write(char *buffer, size_t buffer_length, VFS *vfs) {
	return 0;
}

//------------------------
//Direcotry handle methods
//------------------------

size_t Directory_Handle::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	std::vector<Mft_Item*> directory_items = Functions::Get_Items_In_Directory(vfs, this->uid);
	size_t writed = 0;

	Mft_Item *item = Functions::Get_Mft_Item(vfs, this->uid);
	printf("%s %zd\n", item->item_name, item->item_size);

	for (size_t i = 0; i < directory_items.size(); i++) {
		printf("%s\n", directory_items.at(i)->item_name);
	}

	return writed;
}

//------------------------
//---Pipe handle methods--
//------------------------

size_t Pipe_Handle::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	return 0;
}

size_t Pipe_Handle::Write(char *buffer, size_t buffer_length, VFS *vfs) {
	return 0;
}

void Pipe_Handle::Close() {

}