#include "header.h"

bool VFS::Load_MFT(std::unique_ptr<VFS>& vfs) {

	std::vector<unsigned char> sector(this->boot_record->cluster_size);
	Functions::Process_Sectors(kiv_hal::NDisk_IO::Read_Sectors, this->drive_id, 1, this->boot_record->mft_start_cluster, static_cast<void*>(sector.data()));
	auto buffer = reinterpret_cast<char*>(sector.data());
	std::vector<char> mft_buffer(sizeof(MFT));

	memcpy(mft_buffer.data(), buffer, sizeof(MFT));

	this->mft = reinterpret_cast<MFT*>(mft_buffer.data());

	if (this->mft->size < 1) {
		return false;
	}
	
	this->mft_items;

	for (size_t i = 0; i < this->mft->size; i++) {
		Functions::Process_Sectors(kiv_hal::NDisk_IO::Read_Sectors, this->drive_id, 1, this->boot_record->mft_start_cluster + i + 1, static_cast<void*>(sector.data()));
		buffer = reinterpret_cast<char*>(sector.data());
		char item_buffer[sizeof(Mft_Item)];
		memcpy(item_buffer, buffer, sizeof(Mft_Item));
		Mft_Item *item = reinterpret_cast<Mft_Item*>(item_buffer);
		Mft_Item *itm = new Mft_Item(item->uid, item->is_directory, std::string(item->item_name), item->item_size, item->parent_ID);
		Functions::Write_To_Data_Block(vfs, itm);
		this->mft_items.insert(std::pair<size_t, Mft_Item*>(itm->uid, itm));
	}
	return true;
}

VFS::VFS(uint64_t cluster_count, uint16_t cluster_size, int v_drive_id) {
	this->boot_record = std::make_unique<Boot_Record>(cluster_count, cluster_size);
	this->drive_id = v_drive_id;

	size_t count = 2880;

	this->bitmap.resize(count);
	for (size_t i = 0; i < count; i++) {
		this->bitmap.push_back(false);
	}
}

void VFS::Init_VFS(std::unique_ptr<VFS>& vfs) {
	vfs->mft = new MFT();

	// Init root.
	Mft_Item* mftItem = new Mft_Item(0, kiv_os::NFile_Attributes::Directory, "root", 1, -2);
	vfs->mft->UID_counter = 1;
	vfs->mft->size = 1;
	if (Functions::Is_Bitmap_Writable(vfs, mftItem->item_size)) {
		vfs->mft_items.insert(std::pair<size_t, Mft_Item*>(mftItem->uid, mftItem));
		Functions::Write_To_Data_Block(vfs, mftItem);
	}

	Functions::Save_VFS_MFT(vfs);
	Functions::Save_VFS_MFT_Item(vfs, mftItem->uid);
}