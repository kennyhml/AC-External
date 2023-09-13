#include "stdafx.h"
#include "mem.h"


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