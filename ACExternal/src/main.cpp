
#include "stdafx.h"
#include <iostream> 
#include <vector>
#include <Windows.h>

#include "proc.h"
#include "player.h"
#include "weapon/weapon.h"

namespace staticOffsets {

	int entityList = 0x10F4F8;
	int localPlayer = 0x10F4F4;
	int playerCount = 0x10F500;

	int dmgInstruction = 0x29D1F;
}

int main()
{
	DWORD pid = GetProcessID(L"ac_client.exe");
	std::cout << "[+] Client PID: " << pid << "\n";

	uintptr_t moduleBaseAddr = GetModuleBaseAddress(pid, L"ac_client.exe");
	std::cout << "[+] Module base address: 0x" << std::hex << moduleBaseAddr << "\n";

	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

	uintptr_t entityListAddr = FindDMAAddy(hProcess, moduleBaseAddr + staticOffsets::entityList, { 0x0 });
	uintptr_t localPlayerAddr = FindDMAAddy(hProcess, moduleBaseAddr + staticOffsets::localPlayer, { 0x0 });
	intptr_t playerCountAddr = moduleBaseAddr + staticOffsets::playerCount;
	Player localPlayer = LoadPlayer(hProcess, localPlayerAddr);

	int playerCount;
	ReadProcessMemory(hProcess, (BYTE*)playerCountAddr, (BYTE*)&playerCount, sizeof(playerCount), nullptr);

	std::cout << "Player count: " << playerCount << "\n";
	auto players = LoadPlayers(hProcess, playerCount, entityListAddr);

	for (Player p : players)
	{
		std::cout << p.name << "\n";

	}

	ToggleSpeedHack(hProcess, moduleBaseAddr, false);
}