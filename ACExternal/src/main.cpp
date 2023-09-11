
#include "proc.h"
#include <iostream> 


int main()
{
	DWORD pid = GetProcessID(L"ShooterGame.exe");
	uintptr_t addr = GetModuleBaseAddress(pid, L"ShooterGame.exe");

	std::cout << "[+] Client PID: " << pid << "\n";
	std::cout << "[+] Module base address: 0x" << std::hex << addr << "\n";


	std::cin.get();


}