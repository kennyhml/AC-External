#include "proc.h"
#include <iostream>

/**
 * Finds a target process by name in a process snapshot.
 *
 * @param hSnap The handle to the process snapshot.
 * @param name The name of the target process (UNICODE).
 * 
 * @return A pointer to the struct of the found process (or a nullptr)
 */
static PROCESSENTRY32* findProcessInSnapshot(HANDLE hSnap, const wchar_t* name)
{	
	// PROCESSENTRY32: an entry of the processes in the address space of the snapshot.
	// Essentially a buffer to later push each process into, which is why we need to set
	// the size beforehand. If we dont then Process32First will fail.
	static PROCESSENTRY32 currProcess;
	currProcess.dwSize = sizeof(currProcess);

	if (!Process32First(hSnap, &currProcess))
	{
		std::cout << "Process32First failed. Error: " << GetLastError() << "\n";
		return nullptr;
	}

	// Repeatedly push the next process in the snapshot into the variable
	// And check whether we found the target process.
	do
	{
		std::wcout << "[+] Found " << currProcess.szExeFile << "\n";
		if (!_wcsicmp(currProcess.szExeFile, name))
		{
			return &currProcess;
		}

	} while (Process32Next(hSnap, &currProcess));
		
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
DWORD getProcessID(const wchar_t* name)
{
	// TH32CS_SNAPPROCESS: Include all processes in the system.
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32* process = nullptr;
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		process = findProcessInSnapshot(hSnap, name);
	}

	// Get the ID from the process pointer, unless we got a nullptr
	DWORD processID = process == NULL ? 0 : process->th32ProcessID;

	if (!processID)
	{ 
		std::wcout << "\t[+] " << name << " not found!" << "\n"; 
	}
	else 
	{
		std::cout << "\t[+] Target process found! ID: " << processID << "\n";
	}

	CloseHandle(hSnap);
	return processID;
}


uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	return 0;

}

uintptr_t FIndDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>)
{
	return 0;

}