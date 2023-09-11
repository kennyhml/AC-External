#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>


DWORD getProcessID(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FIndDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>);


