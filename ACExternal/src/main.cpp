
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

	int speedW = 0x5BEA0;
	int speedA = 0x5BE40;
	int speedS = 0x5BF00;
	int speedD = 0x5BF60;

	int dmg = 0x29D1F;


}

int main()
{
	DWORD pid = GetProcessID(L"ac_client.exe");
	std::cout << "[+] Client PID: " << pid << "\n";

	uintptr_t moduleBaseAddr = GetModuleBaseAddress(pid, L"ac_client.exe");
	std::cout << "[+] Module base address: 0x" << std::hex << moduleBaseAddr << "\n";

	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);


	uintptr_t localPlayerAddr = FindDMAAddy(hProcess, moduleBaseAddr + 0x10F4F4, { 0x0 });
	Player localPlayer = LoadPlayer(hProcess, localPlayerAddr);

	localPlayer.setHealth(hProcess, -8989);
	localPlayer.setArmor(hProcess, 101001);

	char newName[16] = "Working easy";
	localPlayer.setName(hProcess, newName);

	std::cin.get();
}