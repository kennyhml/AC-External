
#include "proc.h"
#include <iostream> 


int main()
{
	DWORD pid = GetProcessID(L"ac_client.exe");
	uintptr_t addr = GetModuleBaseAddress(pid, L"ac_client.exe");

	std::cout << "[+] Client PID: " << pid << "\n";
	std::cout << "[+] Module base address: " << addr << "\n";


	std::cin.get();


}