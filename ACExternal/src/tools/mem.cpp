#include "mem.h"
#include "../stdafx.h"


void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{

	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);

}

void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{

	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);
	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;


}

uintptr_t FindDMAAddy(HANDLE hProcess, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i)
	{
		ReadProcessMemory(hProcess, (BYTE*)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}

uintptr_t GetPointedAddress(HANDLE hProcess, uintptr_t pointerAddress)
{
	uintptr_t address;
	ReadProcessMemory(hProcess, (BYTE*)pointerAddress, &address, sizeof(address), nullptr);
	return address;
}
