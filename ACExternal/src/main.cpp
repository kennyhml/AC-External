
#include "stdafx.h"
#include <iostream> 
#include <vector>
#include <Windows.h>

#include "proc.h"
#include "player.h"
#include "weapon.h"


namespace staticOffsets {

	int entityList = 0x10F4F8;
	int localPlayer = 0x10F4F4;
	int playerCount = 0x10F500;

	int speedW = 0x5BEA0;
	int speedA = 0x5BE40;
	int speedS = 0x5BF00;
	int speedD = 0x5BF60;

}

Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddress)
{
	Player player;
	ReadProcessMemory(hProcess, (BYTE*)playerAddress, &player, sizeof(Player), nullptr);
	return player;

}

std::vector<Player> GetPlayers(HANDLE hProcess, uintptr_t modBaseAddr)
{

	uintptr_t playerCountPtr = modBaseAddr + staticOffsets::playerCount;
	uintptr_t entityListPtr = modBaseAddr + staticOffsets::entityList;

	int playerCount = 0;
	ReadProcessMemory(hProcess, (BYTE*)playerCountPtr, &playerCount, sizeof(playerCount), nullptr);
	std::cout << "[+] Current player count: " << std::dec << playerCount << "\n";
	playerCount--;

	uintptr_t entityListAddr;
	std::vector<Player> players(playerCount);

	ReadProcessMemory(hProcess, (BYTE*)entityListPtr, &entityListAddr, sizeof(entityListAddr), nullptr);
	std::cout << "[+] Entity list at " << std::hex << entityListAddr << "\n";

	for (int i = 1; i < playerCount + 1; i++)
	{
		int offset = 4 * i;
		uintptr_t playerAddress;
		ReadProcessMemory(hProcess, (BYTE*)entityListAddr + offset, &playerAddress, sizeof(playerAddress), nullptr);
		players.push_back(LoadPlayer(hProcess, playerAddress));
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
	Weapon wp = LoadWeapon(hProcess, 0x01191B90);


	wp.data->setReloadTime(hProcess, 0);


	// Player localPlayer = LoadPlayer(hProcess, FindDMAAddy(hProcess, moduleBaseAddr + 0x10F4F4, { 0x0 }));

	// auto players = GetPlayers(hProcess, moduleBaseAddr);


	std::cin.get();
}