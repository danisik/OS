#include "test.h"
#include <sstream>

size_t __stdcall test(const kiv_hal::TRegisters &regs) {
	const kiv_os::THandle std_out = static_cast<kiv_os::THandle>(regs.rbx.x);

	kiv_os::NOpen_File flags = static_cast<kiv_os::NOpen_File>(0);
	const int n = 10;

	/*
	std::vector<kiv_os::THandle> handles;

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
	*/
	
	// Write into file.
	/*
	kiv_os::THandle handle;
	const char *file_name = reinterpret_cast<const char*>(regs.rdi.r);
	kiv_os_rtl::Open_File(file_name, flags, kiv_os::NFile_Attributes::System_File, handle);

	char buffer[2305] = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Nulla pulvinar eleifend sem. Aliquam ornare wisi \neu metus. Vestibulum fermentum tortor id mi. Duis ante orci, molestie vitae vehicula venenatis, tincidunt ac pede. Et harum quidem rerum facilis est et expedita distinctio. Nullam sapien sem, ornare\n ac, nonummy non, lobortis a enim. Duis bibendum, lectus ut viverra rhoncus, dolor nunc faucibus libero, eget facilisis enim ipsum id lacus. Integer vulputate sem a nibh rutrum consequat. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Cras pede libero, dapibus nec, pretium sit amet, tempor quis. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, \nsed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem.Nulla non arcu lacinia neque faucibus fringilla. Nulla turpis magna, cursus sit amet, suscipit a, interdum id, felis. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat. Aliquam erat volutpat. Duis viverra diam non justo. Fusce wisi. Maecenas sollicitudin. Nullam eget nisl. Duis bibendum, lectus ut viverra rhoncus, dolor nunc faucibus libero, eget facilisis enim ipsum id lacus. Class aptent \ntaciti sociosqu ad litora torquent per conubia nostra, per inceptos hymenaeos. Etiam dictum tincidunt diam. Pellentesque arcu. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Donec iaculis gravida nulla.Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Etiam posuere lacus quis dolor. \nProin in tellus sit amet nibh dignissim sagittis. Nullam at arcu a est sollicitudin euismod. Aliquam in lorem sit amet leo accumsan lacinia. Nunc dapibus tortor vel mi dapibus sollicitudin. Duis condimentum augue id magna semper rutrum. Nullam sit amet magna in magna gravida vehicula. Pellentesque sapien. Pellentesque pretium lectus id turpis. Duis condimentum \naugue id magna semper rutrum.";
	size_t written = 0;
	kiv_os_rtl::Write_File(handle, buffer, sizeof(buffer), written);
	*/

	kiv_os::THandle pipe_handles[2];
	kiv_os_rtl::Create_Pipe(pipe_handles);

	size_t s = 0;
	char in_buffer[20] = "aalfabetagamadelta\n";
	char out_buffer[512];
	kiv_os_rtl::Write_File(pipe_handles[0], in_buffer, 20, s);
	kiv_os_rtl::Read_File(pipe_handles[1], out_buffer, 512, s);

	std::string str;
	str.append(out_buffer, s);
	kiv_os_rtl::Write_File(std_out, str.data(), str.size(), s);

	kiv_os_rtl::Exit(0);
	return 0;
}