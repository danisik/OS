#include "io_handle.h"

//------------------------
//-----Handle methods-----
//------------------------

size_t IO_Handle::Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
	// Do nothing.
	return 0;
}

size_t IO_Handle::Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
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

size_t STD_Handle_In::Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
	kiv_hal::TRegisters registers;

	size_t pos = 0;
	while (pos < buffer_length) {
		registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NKeyboard::Read_Char);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Keyboard, registers);

		
		if (!registers.flags.non_zero) break;	// Nic jsme neprecetli, 
												// pokud je rax.l EOT, pak byl zrejme vstup korektne ukoncen
												// jinak zrejme doslo k chybe zarizeni.

		char ch = registers.rax.l;

		// Osetrime zname kody.
		switch (static_cast<kiv_hal::NControl_Codes>(ch)) {
		case kiv_hal::NControl_Codes::BS: {
			// Mazeme znak z bufferu.
			if (pos > 0) pos--;

			registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_Control_Char);
			registers.rdx.l = ch;
			kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
		}
										  break;

		case kiv_hal::NControl_Codes::LF:  break;		// Jenom pohltime, ale necteme.
		case kiv_hal::NControl_Codes::NUL: return 0;	// CTRL+Z.
		case kiv_hal::NControl_Codes::CR: {				// Add new line if enter is pressed.
			buffer[pos] = '\n';
			registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_String);
			registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(&buffer[pos]);
			registers.rcx.r = 1;
			kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
			pos++;
			return pos;
		}
		default: {
			buffer[pos] = ch;
			pos++;
			registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_String);
			registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(&ch);
			registers.rcx.r = 1;
			kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
			break;
		}
		}
	}

	return pos;
}

size_t STD_Handle_Out::Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
	kiv_hal::TRegisters registers;
	registers.rax.h = static_cast<decltype(registers.rax.h)>(kiv_hal::NVGA_BIOS::Write_String);
	registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(buffer);
	registers.rcx.r = buffer_length;
	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);

	return (size_t)registers.rcx.r;
}

//------------------------
//---Pipe handle methods--
//------------------------

Pipe_Handle::Pipe_Handle(Pipe *p_pipe) {
	pipe = p_pipe;
	function = Pipe_Function::Write;
}

Pipe_Handle::Pipe_Handle(int p_buffer_size) {
	pipe = new Pipe(p_buffer_size);
	function = Pipe_Function::Read;
}

size_t Pipe_Handle::Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
	size_t read = 0;
	read = pipe->Consume(buffer, buffer_length);
	return read;
}

size_t Pipe_Handle::Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
	size_t written = 0;
	written = pipe->Produce(buffer, buffer_length);
	return written;
}

void Pipe_Handle::Close() {
	pipe->Close(function);

	if (pipe->closed_out && pipe->closed_in) {
		delete pipe->consumer;
		delete pipe->producer;
		delete pipe;
	}
}

//------------------------
//---File handle methods--
//------------------------

size_t File_Handle::Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {

	if ((seek - 1) >= item->item_size) {
		return 0;
	}

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
		if ((seek - 1) + buffer_length > vfs->boot_record->cluster_size) item_current_sector_count++;

		// If current sector count is more or equal of current_fragment_sector_count, move to next fragment.
		if (item_current_sector_count >= item->fragment_cluster_count[item_current_sector_position]) {
			item_current_sector_position++;
			item_current_sector_count = 0;
		}

		if (item->item_size < buffer_length) {
			memcpy(buffer + actual_buffer_position, read_buffer, item->item_size);

			actual_buffer_position += item->item_size;
			break;
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

size_t File_Handle::Write(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
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

size_t Directory_Handle::Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
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
//--Procfs handle methods-
//------------------------

size_t Procfs_Handle::Read(char *buffer, size_t buffer_length, std::unique_ptr<VFS>& vfs, std::unique_ptr<IO_Process>& io_process) {
	std::map<size_t, std::unique_ptr<Process>>::iterator it_process = io_process->processes.begin();
	
	size_t actual_buffer_position = 0;
	std::stringstream ss;
	while (it_process != io_process->processes.end()) {		
		ss << it_process->second->name << " \t\t " << it_process->first << "\n";

		it_process++;
	}

	memcpy(buffer, ss.str().data(), (size_t)ss.tellp());

	return (size_t)ss.tellp();
}