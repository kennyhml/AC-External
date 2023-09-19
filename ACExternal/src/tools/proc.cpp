#include "proc.h"
#include <memory>
#include <optional>


/**
 * Finds a target process by name in a process snapshot.
 *
 * @param hSnap The handle to the process snapshot.
 * @param name The name of the target process (UNICODE).
 *
 * @return The PROCESSENTRY32 struct of the process if found.
 */
static std::optional<PROCESSENTRY32> FindProcessInSnapshot(HANDLE hSnap, const char* name)
{
	// PROCESSENTRY32: an entry of the processes in the address space of the snapshot.
	// Essentially a buffer to later push each process into, which is why we need to set
	// the size beforehand. If we dont then Process32First will fail.
	PROCESSENTRY32 currProcess;
	currProcess.dwSize = sizeof(currProcess);

	if (!Process32First(hSnap, &currProcess))
	{
		std::cout << "Process32First failed. Error: " << GetLastError() << "\n";
		return std::nullopt;
	}

	// Repeatedly push the next process in the snapshot into the variable
	// And check whether we found the target process.
	do
	{
		if (!_stricmp(currProcess.szExeFile, name)) return currProcess;
	} while (Process32Next(hSnap, &currProcess));

	return std::nullopt;
}

/**
 * Finds a target module by name in a process.
 *
 * @param hSnap The handle to the process snapshot.
 * @param name The name of the target module (UNICODE).
 *
 * @return The MODULEENTRY32 struct of the module if found.
 */
static std::optional<MODULEENTRY32> FindModuleInSnapshot(HANDLE hSnap, const char* name)
{
	// MODULEENTRY32: an entry of the modules in the address space of the snapshot.
	// Essentially a buffer to later push each module into, which is why we need to set
	// the size beforehand. If we dont then MODULEENTRY32 will fail.
	MODULEENTRY32 currModule;
	currModule.dwSize = sizeof(currModule);

	if (!Module32First(hSnap, &currModule))
	{
		std::cout << "Module32First failed. Error: " << GetLastError() << "\n";
		return std::nullopt;
	}
	// Repeatedly push the next module in the snapshot into the variable
	// And check whether we found the target module.
	do
	{
		if (!_stricmp(currModule.szModule, name)) return currModule;
	} while (Module32Next(hSnap, &currModule));

	return std::nullopt;
}

/**
 * Gets the process ID of a given process by name.
 * Name must be passed as a wide-char as PROCESSENTRY32 stores it's as such.
 *
 * @param name The name of the target process.
 *
 * @return The PID (Process ID) if the process exists, else 0.
 */
DWORD GetProcessID(const char* name)
{
	// TH32CS_SNAPPROCESS: Include all processes in the system.
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	std::optional<PROCESSENTRY32> proc = std::nullopt;

	if (hSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "[+] ERROR GetProcessID: CreateToolhelp32Snapshot - Error Code: " << GetLastError() << "\n";
	}
	else
	{
		proc = FindProcessInSnapshot(hSnap, name);
		CloseHandle(hSnap);
	}
	return proc == std::nullopt ? 0 : proc->th32ProcessID;
}

/**
 * Gets the base address of a given module by name.
 * Name must be passed as a wide-char as MODULEENTRY32 stores it's as such.
 *
 * @param name The name of the target module.
 *
 * @return The base address if the module exists, else 0.
 */
uintptr_t GetModuleBaseAddress(DWORD pid, const char* name)
{
	// TH32CS_SNAPMODULE alone will exclude 32-bit modules on a 64-bit process
	DWORD dwFlags = TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32;
	HANDLE hSnap = CreateToolhelp32Snapshot(dwFlags, pid);
	std::optional<MODULEENTRY32> mod = std::nullopt;

	if (hSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "[+] ERROR GetProcessID: CreateToolhelp32Snapshot - Error Code: " << GetLastError() << "\n";
	}
	else
	{
		mod = FindModuleInSnapshot(hSnap, name);
		CloseHandle(hSnap);
	}
	return mod == std::nullopt ? 0 : (uintptr_t)(mod->modBaseAddr);
};