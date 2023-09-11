#include "proc.h"
#include <iostream>
#include <memory>

/**
 * Finds a target process by name in a process snapshot.
 *
 * @param hSnap The handle to the process snapshot.
 * @param name The name of the target process (UNICODE).
 *
 * @return A pointer to the struct of the found process (or a nullptr)
 */
static std::unique_ptr<PROCESSENTRY32> FindProcessInSnapshot(HANDLE hSnap, const wchar_t* name)
{
	// PROCESSENTRY32: an entry of the processes in the address space of the snapshot.
	// Essentially a buffer to later push each process into, which is why we need to set
	// the size beforehand. If we dont then Process32First will fail.
	std::unique_ptr<PROCESSENTRY32> currProcess = std::make_unique<PROCESSENTRY32>();
	currProcess->dwSize = sizeof(*currProcess);

	if (!Process32First(hSnap, currProcess.get()))
	{
		std::cout << "Process32First failed. Error: " << GetLastError() << "\n";
		return nullptr;
	}

	// Repeatedly push the next process in the snapshot into the variable
	// And check whether we found the target process.
	do
	{
		if (!_wcsicmp(currProcess->szExeFile, name)) return currProcess;
	} while (Process32Next(hSnap, currProcess.get()));

	return nullptr;
}

/**
 * Finds a target module by name in a process.
 *
 * @param hSnap The handle to the process snapshot.
 * @param name The name of the target module (UNICODE).
 *
 * @return A pointer to the struct of the found module (or a nullptr)
 */
static std::unique_ptr<MODULEENTRY32> FindModuleInSnapshot(HANDLE hSnap, const wchar_t* name)
{
	// MODULEENTRY32: an entry of the modules in the address space of the snapshot.
	// Essentially a buffer to later push each module into, which is why we need to set
	// the size beforehand. If we dont then MODULEENTRY32 will fail.
	std::unique_ptr<MODULEENTRY32> currModule = std::make_unique<MODULEENTRY32>();
	currModule->dwSize = sizeof(*currModule);

	if (!Module32First(hSnap, currModule.get()))
	{
		std::cout << "Module32First failed. Error: " << GetLastError() << "\n";
		return nullptr;
	}

	// Repeatedly push the next module in the snapshot into the variable
	// And check whether we found the target module.
	do
	{
		if (!_wcsicmp(currModule->szModule, name)) return currModule;

	} while (Module32Next(hSnap, currModule.get()));

	return nullptr;
}

/**
 * Gets the process ID of a given process by name.
 * Name must be passed as a wide-char as PROCESSENTRY32 stores it's as such.
 *
 * @param name The name of the target process.
 *
 * @return The PID (Process ID) if the process exists, else 0.
 */
DWORD GetProcessID(const wchar_t* name)
{
	// TH32CS_SNAPPROCESS: Include all processes in the system.
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	std::unique_ptr<PROCESSENTRY32> proc = hSnap != INVALID_HANDLE_VALUE ?
		FindProcessInSnapshot(hSnap, name) : nullptr;

	// Get the ID from the process pointer & clean up.
	DWORD processID = proc == NULL ? 0 : proc->th32ProcessID;
	CloseHandle(hSnap);

	return processID;
}

uintptr_t GetModuleBaseAddress(DWORD pid, const wchar_t* name) 
{
	// TH32CS_SNAPMODULE alone will exclude 32-bit modules on a 64-bit process
	DWORD dwFlags = TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32;
	HANDLE hSnap = CreateToolhelp32Snapshot(dwFlags, pid);

	std::unique_ptr<MODULEENTRY32> mod = hSnap != INVALID_HANDLE_VALUE ?
		FindModuleInSnapshot(hSnap, name) : nullptr;

	// Get the base address from the module pointer & clean up
	uintptr_t baseAddr = mod == NULL ? 0 : (uintptr_t)mod->modBaseAddr;
	CloseHandle(hSnap);

	return baseAddr;
};

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>)
{
	return 0;

}