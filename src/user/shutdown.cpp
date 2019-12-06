#include "shutdown.h"

size_t __stdcall shutdown(const kiv_hal::TRegisters &regs) {
	kiv_os_rtl::Shutdown();
	kiv_os_rtl::Exit(0);
	printf("shutdowned\n");
	return 0;
}