
#include "stdafx.h"
#include <iostream> 
#include <vector>
#include <Windows.h>

#include "proc.h"
#include "player.h"
#include "offsets.h"


Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddr)
{
	uintptr_t playerLocation;
	ReadProcessMemory(hProcess, (BYTE*)playerAddr, &playerLocation, sizeof(playerLocation), nullptr);

	Player player(playerLocation, hProcess);
	std::cout << "\t[+] Loaded player " << player.name << " at " << std::hex << playerLocation << "!\n";
	return player;
}


std::vector<Player> GetPlayers(HANDLE hProcess, uintptr_t modBaseAddr)
{

	uintptr_t playerCountPtr = modBaseAddr + offsets::playerCount;
	uintptr_t entityListPtr = modBaseAddr + offsets::entityList;

	int playerCount = 0;
	ReadProcessMemory(hProcess, (BYTE*)playerCountPtr, &playerCount, sizeof(playerCount), nullptr);
	std::cout << "[+] Current player count: " << std::dec << playerCount << "\n";
	playerCount--;

	uintptr_t entityListAddr;
	std::vector<Player> players;
	ReadProcessMemory(hProcess, (BYTE*)entityListPtr, &entityListAddr, sizeof(entityListAddr), nullptr);

	std::cout << "[+] Entity list at " << std::hex << entityListAddr << "\n";

	for (int i = 1; i < playerCount + 1; i++) 
	{
		int offset = 4 * i;
		uintptr_t entityPtr = entityListAddr + offset;
		LoadPlayer(hProcess, entityPtr);


	}
	return players;

}







int main()
{
	DWORD pid = GetProcessID(L"ac_client.exe");
	std::cout << "[+] Client PID: " << pid << "\n";

	uintptr_t moduleBaseAddr = GetModuleBaseAddress(pid, L"ac_client.exe");
	std::cout << "[+] Module base address: 0x" << std::hex << moduleBaseAddr << "\n";

	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);


	GetPlayers(hProcess, moduleBaseAddr);
	return 0;





	uintptr_t dynamicPtrBaseAddr = moduleBaseAddr + 0x10f4f4;
	std::cout << "[+] DynamicPtrBaseAddr: 0x" << std::hex << dynamicPtrBaseAddr << "\n";

	uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, { 0x374, 0x14, 0x0 });
	std::cout << "[+] AmmoAddr: 0x" << std::hex << ammoAddr << "\n";

	int ammoValue = 0;
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	std::cout << "[+] Current Ammo: " << std::dec << ammoValue << "\n";

	int newAmmo = 1337;
	WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);
	std::cout << "[+] Ammo set to " << newAmmo << "\n";

	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	std::cout << "[+] New ammo: " << newAmmo << "\n";

	std::cin.get();
}