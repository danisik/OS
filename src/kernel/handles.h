#pragma once

#include "..\api\api.h"

#include <Windows.h>
#include <mutex>


kiv_os::THandle Convert_Native_Handle(const HANDLE hnd);
HANDLE Resolve_kiv_os_Handle(const kiv_os::THandle hnd);
bool Remove_Handle(const kiv_os::THandle hnd);