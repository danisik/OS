#include "shutdown.h"

extern bool shutdown_signalized;

size_t __stdcall shutdown(const kiv_hal::TRegisters &regs) {
	shutdown_signalized = true;
	kiv_os_rtl::Shutdown();
	return 0;
}