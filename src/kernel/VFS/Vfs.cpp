#include "header.h"

void VFS::Load_MFT() {

}

void VFS::Load_Bitmap() {
	kiv_hal::TRegisters regs;

	std::vector<unsigned char> sector(this->boot_record->cluster_size * this->boot_record->cluster_count);
	kiv_hal::TDisk_Address_Packet dap;
	dap.count = this->boot_record->cluster_count;
	dap.sectors = static_cast<void*>(sector.data());
	dap.lba_index = this->boot_record->bitmap_start_cluster;
	regs.rdi.r = reinterpret_cast<decltype(regs.rdi.r)>(&dap);

	regs.rax.h = static_cast<uint8_t>(kiv_hal::NDisk_IO::Read_Sectors);		// Read boot block.
	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Disk_IO, regs);
}

void VFS::Load_VFS() {
	Load_MFT();
	Load_Bitmap();
}

VFS::VFS(uint64_t cluster_count, uint16_t cluster_size){
    this->boot_record = new Boot_Record(cluster_count, cluster_size);
    this->mft = new MFT();
    this->bitmap = new bool[boot_record->Get_Cluster_Count()];
    for (int i = 0; i<this->boot_record->Get_Cluster_Count(); i++) {
        this->bitmap[i] = false;
    }

	// Init root.
	Mft_Item* mftItem = new Mft_Item(-1, kiv_os::NFile_Attributes::Directory, "", 1, -2, false, -1);
	if (Functions::Is_Bitmap_Writable(this, mftItem->item_size)) {
		Functions::Write_To_Data_Block(this, mftItem);
	}

}