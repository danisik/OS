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
			seek = position + 1;
			break;
		case kiv_os::NFile_Seek::Current:
			seek += position + 1;	
			break;
		case kiv_os::NFile_Seek::End:
			seek = size - position;
			break;
	}

	if (seek < 1) {
		seek = 1;
	}

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

	size_t actual_buffer_position = 0;

	size_t sectors_count = item->item_size / vfs->boot_record->cluster_size;
	if (item->item_size % vfs->boot_record->cluster_size != 0) sectors_count++;

	size_t seek_sector = (seek - 1) / vfs->boot_record->cluster_size;

	size_t item_current_sector_count = 0;
	size_t item_current_sector_position = 0;

	size_t sector_position = 0;
	while (1) {
		if (sector_position < seek_sector) {
			sector_position++;
			item_current_sector_count++;
		}
		else {
			break;
		}

		if (item->fragment_cluster_count[item_current_sector_position] == item_current_sector_count) {
			item_current_sector_count = 0;
			item_current_sector_position++;
		}
	}

	for (size_t i = seek_sector; i < sectors_count; i++) {
		
		if (actual_buffer_position >= buffer_length) {
			break;
		}

		std::vector<unsigned char> read_buffer_vector(vfs->boot_record->cluster_size);
		// Read from fragment_start_sector + current_sector_position.
		Functions::Process_Sectors(kiv_hal::NDisk_IO::Read_Sectors, vfs->drive_id, 1, item->fragment_start_cluster[item_current_sector_position] + item_current_sector_count, static_cast<void*>(read_buffer_vector.data()));		
		auto read_buffer = reinterpret_cast<char*>(read_buffer_vector.data());

		// Move to next sector.
		item_current_sector_count++;

		// If current sector count is more or equal of current_fragment_sector_count, move to next fragment.
		if (item_current_sector_count >= item->fragment_cluster_count[item_current_sector_position]) {
			item_current_sector_position++;
			item_current_sector_count = 0;
		}

		if (actual_buffer_position + vfs->boot_record->cluster_size > buffer_length) {
			size_t difference = actual_buffer_position + vfs->boot_record->cluster_size - buffer_length;
			memcpy(buffer + actual_buffer_position, read_buffer, difference);

			actual_buffer_position += difference;
			break;
		}

		// If currently processed sector is seek sektor, calculate needed size to fill the buffer
		if (i == (sectors_count - 1)) {
			size_t specific_size_in_seek_sector = item->item_size % vfs->boot_record->cluster_size;
			memcpy(buffer + actual_buffer_position, read_buffer, specific_size_in_seek_sector);

			actual_buffer_position += specific_size_in_seek_sector;
		}
		else {
			memcpy(buffer + actual_buffer_position, read_buffer, vfs->boot_record->cluster_size);
			actual_buffer_position += vfs->boot_record->cluster_size;
		}
	}
	return actual_buffer_position;
}

size_t File_Handle::Write(char *buffer, size_t buffer_length, VFS *vfs) {
	size_t actual_buffer_position = 0;
	seek = 1;

	Functions::Remove_From_Data_Block(vfs, item);
	item->item_size = buffer_length;
	Functions::Write_To_Data_Block(vfs, item);
	Functions::Save_VFS_MFT_Item(vfs, item->uid);

	bool last_sector_not_full = false;
	size_t sectors_count = buffer_length / vfs->boot_record->cluster_size;
	if (buffer_length % vfs->boot_record->cluster_size != 0) {
		sectors_count++;
		last_sector_not_full = true;
	}
	
	size_t buffer_size = vfs->boot_record->cluster_size;

	size_t item_current_sector_count = 0;
	size_t item_current_sector_position = 0;

	for (size_t i = 0; i < sectors_count; i++) {

		if (i == (sectors_count - 1) && last_sector_not_full) {
			buffer_size = buffer_length % vfs->boot_record->cluster_size;
		}

		std::vector<unsigned char> write_buffer(buffer_size);
		memcpy(write_buffer.data(), buffer + actual_buffer_position, buffer_size);
		Functions::Process_Sectors(kiv_hal::NDisk_IO::Write_Sectors, vfs->drive_id, 1, item->fragment_start_cluster[item_current_sector_position] + item_current_sector_count, static_cast<void*>(write_buffer.data()));
		item_current_sector_count++;
		
		// If current sector count is more or equal of current_fragment_sector_count, move to next fragment.
		if (item_current_sector_count >= item->fragment_cluster_count[item_current_sector_position]) {
			item_current_sector_position++;
			item_current_sector_count = 0;
		}

		actual_buffer_position += buffer_size;
 	}
	return actual_buffer_position;
}

//------------------------
//Direcotry handle methods
//------------------------

size_t Directory_Handle::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	std::vector<Mft_Item*> directory_items = Functions::Get_Items_In_Directory(vfs, this->item->uid);

	size_t actual_buffer_position = 0;

	for (size_t i = (seek - 1); i < directory_items.size(); i++) {
		if (actual_buffer_position >= buffer_length) {
			break;
		}

		Mft_Item* item = directory_items.at(i);

		kiv_os::TDir_Entry entry;

		entry.file_attributes = static_cast<uint16_t>(item->is_directory);
		strcpy_s(entry.file_name, item->item_name);

		memcpy(buffer + actual_buffer_position, &entry, sizeof(kiv_os::TDir_Entry));
		actual_buffer_position += sizeof(kiv_os::TDir_Entry);
	}

	return actual_buffer_position;
}

//------------------------
//---Pipe handle methods--
//------------------------

size_t Pipe_Handle::Read(char *buffer, size_t buffer_length, VFS *vfs) {
	// TODO Pipe_Handle::Read: Functional code.
	return 0;
}

size_t Pipe_Handle::Write(char *buffer, size_t buffer_length, VFS *vfs) {
	// TODO Pipe_Handle::Write: Functional code.
	return 0;
}

void Pipe_Handle::Close() {
	// TODO Pipe_Handle::Close: Functional code.
}