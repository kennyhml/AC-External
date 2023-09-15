#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>


DWORD GetProcessID(const char* procName);

uintptr_t GetModuleBaseAddress(DWORD pid, const char* modName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);


