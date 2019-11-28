#include "test.h"
#include <sstream>

size_t __stdcall test(const kiv_hal::TRegisters &regs) {


	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(0);
	std::vector<kiv_os::THandle> handles;
	int n = 100;

	printf("\nCreating directories and files\n");
	for (size_t i = 0; i < n; i++) {
		kiv_os::THandle handle;
		std::stringstream ss;
		ss.clear();
		ss << i;
		std::string file_name;
		ss >> file_name;
		ss.clear();
		printf("%s", file_name.c_str());
		if (i < n / 2) {
			printf(" directory\n");
			kiv_os_rtl::Open_File(file_name.c_str(), flags, kiv_os::NFile_Attributes::Directory, handle);
		}
		else {
			printf(" file\n");
			kiv_os_rtl::Open_File(file_name.c_str(), flags, kiv_os::NFile_Attributes::System_File, handle);
		}
		handles.push_back(handle);
	}
	printf("\nEnd creating directories and files\n");
	
	printf("\nRemoving directories and files\n");
	for (size_t i = 0; i < n; i++) {
		std::stringstream ss;
		ss.clear();
		ss << i;
		std::string file_name;
		ss >> file_name;
		ss.clear();
		printf("%s\n", file_name.c_str());
		kiv_os_rtl::Delete_File(file_name.c_str());
	}
	printf("\nEnd removing directories and files\n");
	
	kiv_os_rtl::Exit(0);
	return 0;
}