#include "header.h"

VFS::VFS(uint64_t cluster_count, uint16_t cluster_size, int v_drive_id)
{
	this->boot_record = std::make_unique<Boot_Record>(cluster_count, cluster_size);
	this->drive_id = v_drive_id;

	size_t count = 2880;

	this->bitmap.resize(count);
	for (size_t i = 0; i < count; i++) 
	{
		this->bitmap.push_back(false);
	}
}

void VFS::Init_VFS(std::unique_ptr<VFS>& vfs) 
{
	vfs->mft = new MFT();

	// Init root.
	Mft_Item* mftItem = new Mft_Item(0, kiv_os::NFile_Attributes::Directory, "root", 1, -2);
	vfs->mft->UID_counter = 1;
	vfs->mft->size = 1;
	if (Functions::Is_Bitmap_Writable(vfs, mftItem->item_size)) 
	{
		vfs->mft_items.insert(std::pair<size_t, Mft_Item*>(mftItem->uid, mftItem));
		Functions::Write_To_Data_Block(vfs, mftItem);
	}

	Functions::Save_VFS_MFT(vfs);
	Functions::Save_VFS_MFT_Item(vfs, mftItem->uid);
}