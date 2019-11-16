#pragma once

#include <Windows.h>
#include "process.h"
#include <map>

extern "C" HMODULE User_Programs;

// Index -> process_ID; Value -> Process.
extern "C" std::map<size_t, std::unique_ptr<Process>> processes;
// Index -> thread_ID; Value -> process_ID
extern "C" std::map<size_t, size_t> thread_ID_to_process_ID;
// Index -> handle of thread / process; Value -> thread_ID.
extern "C" std::map<kiv_os::THandle, size_t> t_handle_to_thread_ID;