
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


std::vector<Player> GetPlayers(HANDLE hProcess, uintptr_t modBaseAddr)
{

	uintptr_t playerCountPtr = modBaseAddr + staticOffsets::playerCount;
	uintptr_t entityListPtr = modBaseAddr + staticOffsets::entityList;

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

	}
	return players;

}

/**
 * Loads a weapon object at the given address, including the sub structures such as:
 * - The weapons owner (Player object)
 * - The data of the weapon (weaponData object)
 * - The information about the magazine of the weapon (reserveData object)
 *
 * Note that those sub structs are allocated on the heap and the weapon object receives
 * a pointer to them, when the weapon object is destroyed, the sub structs are freed.
 *
 * @param hProcess The handle to the process to read memory from.
 * @param weaponAddress The direct address of the weapon object.
 *
 * @return A reconstruction of the weapon object located at the given address.
 */
Weapon LoadWeapon(HANDLE hProcess, uintptr_t weaponAddress)
{
	// Load the weapon first, this will provide us with the pointers to the weapon
	// owner, the weapon data and the reserve (magazine) data.
	Weapon weapon;
	ReadProcessMemory(hProcess, (BYTE*)weaponAddress, &weapon, sizeof(Weapon), nullptr);

	// The weapon.owner currently only contains the address of the owner, now we load it
	// and change the .owner pointer to the loaded object
	Player* owner = new Player;
	ReadProcessMemory(hProcess, (BYTE*)weapon.owner, &*owner, sizeof(Player), nullptr);
	weapon.owner = owner;

	// Do the same we did for the owner for weaponData and reserveData
	WeaponData* weaponData = new WeaponData();
	ReadProcessMemory(hProcess, (BYTE*)weapon.data, &*weaponData, sizeof(WeaponData), nullptr);
	weapon.data = weaponData;

	ReserveData* reserveData = new ReserveData();
	ReadProcessMemory(hProcess, (BYTE*)weapon.reserveData, &*reserveData, sizeof(ReserveData), nullptr);
	weapon.reserveData = reserveData;

	return weapon;
}

int main()
{
	DWORD pid = GetProcessID(L"ac_client.exe");
	std::cout << "[+] Client PID: " << pid << "\n";

	uintptr_t moduleBaseAddr = GetModuleBaseAddress(pid, L"ac_client.exe");
	std::cout << "[+] Module base address: 0x" << std::hex << moduleBaseAddr << "\n";

	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);

	std::cin.get();
}