#include "mem.h"


/**
 * @brief Replaces the bytes at the destination with the bytes at the source.
 *
 * @param dst A byte pointer to the memory address we want to patch bytes at.
 * @param src A byte pointer to the memory address containing the data to patch with.
 * @param size The size of the source data to patch with.
 * @param hProcess The handle to the process to perform the patch on.
 */
void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
	// Remember the previous protection level so we can change it back after
	DWORD oldProtect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
}

/**
 * @brief Replaces the bytes at the destination with NOPs (No operations).
 *
 * @param dst A byte pointer to the memory address we want to patch bytes at.
 * @param size The amount of NOPs to place at the destination.
 * @param hProcess The handle to the process to perform the patch on.
 */
void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{
	// Create and populate a byte array of the given size as our source
	BYTE* nopArray = new BYTE[size];
	memset(nopArray, 0x90, size);

	// Patch the NOP byte array in
	PatchEx(dst, nopArray, size, hProcess);
	delete[] nopArray;
}

/**
 * @brief Steps through a vector of offsets to reach the final destination of multi
 * level pointers.
 *
 * @param hProcess The handle to the process to find the destination in.
 * @param ptr The starting address of our multi-level pointer.
 * @param offsets The offset to apply after following each pointer.
 */
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

/**
 * @brief Gets the address an address in memory is pointing to.
 * This is equivalent to calling FindDMAADDy with a single 0x0 offset.
 *
 * @param hProcess The handle to the process to find the destination in.
 * @param pointerAddress The address to get the pointed address of.
 */
uintptr_t GetPointedAddress(HANDLE hProcess, uintptr_t pointerAddress)
{
	uintptr_t address;
	ReadProcessMemory(hProcess, (BYTE*)pointerAddress, &address, sizeof(address), nullptr);
	return address;
}

