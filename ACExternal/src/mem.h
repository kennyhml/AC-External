#pragma once


void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);
void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
