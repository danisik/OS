#include "rtl.h"
#include "shutdown.h"

size_t __stdcall shutdown(const kiv_hal::TRegisters &regs) {
	bool success = kiv_os_rtl::Shutdown();
	if (success) {
		kiv_os_rtl::Exit((uint16_t) Exit_Codes::EXIT_CODE_SUCCESS);
		return (size_t) Exit_Codes::EXIT_CODE_SUCCESS;
	}
	
	return (size_t) Exit_Codes::EXIT_CODE_FAILURE;
}