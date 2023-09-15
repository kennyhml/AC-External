
#include "stdafx.h"
#include <iostream> 
#include <array>
#include <Windows.h>
#include <cstdio>

#include "settings.h"
#include "gui.h"
#include "proc.h"
#include "player.h"
#include "weapon/weapon.h"

#include "geometry.h"
#include <cmath>
#include <unordered_map>

static bool redirectedWeaponPointers = false;

HANDLE GetProcessHandle(DWORD& pid, uintptr_t& modBaseAddress)
{
	while (!pid) 
	{ 
		pid = GetProcessID("ac_client.exe"); 
	}
	std::cout << "[+] Got client process ID: " << pid << "\n";

	while (!modBaseAddress)
	{ 
		modBaseAddress = GetModuleBaseAddress(pid, "ac_client.exe");
	}
	std::cout << "[+] Got module base address: 0x" << std::hex << modBaseAddress << "\n";

	return OpenProcess(PROCESS_ALL_ACCESS, NULL, pid);
}


static std::vector<Weapon*> GetWeapons(Player player)
{
	return std::vector<Weapon*> {
		player.knife, player.pistol,
			player.carbine, player.shotgun,
			player.subgun, player.sniper,
			player.assaultRifle, player.grenade,
			player.akimboPistol
	};
}

void RedirectWeaponPointers(Player& localPlayer, std::vector<Player>& otherPlayers)
{
	auto badWeapons = GetWeapons(localPlayer);
	std::unordered_map<char*, uintptr_t> goodAddresses;

	std::cout << "[+] Bad weapon addresses pointed to by local player:\n";
	for (Weapon* weapon : badWeapons)
	{
		std::cout << "\t[+] Bad weapon pointer ->" << std::hex << weapon->baseAddress << "\n";
	}

	for (Player& p : otherPlayers) 
	{
		std::cout << "[+] Checking weapon pointers of " << p.name << "\n";
		auto weapons = GetWeapons(p);
		for (Weapon* weapon : weapons)
		{
			bool isBad = false;
			for (Weapon* w : badWeapons) 
			{
				if (w->baseAddress == weapon->baseAddress) 
				{
					isBad = true;
					break;
				}
			}
			
			if (!isBad) {
				std::cout << "\t[+] Pointer to " << weapon->data->name << " at " << std::hex << weapon->baseAddress << " is good!\n";
			}
			else 
			{
				std::cout << "\t[+] Pointer to " << weapon->data->name << " at " << std::hex << weapon->baseAddress << " is bad!\n";
			}
		}

	}
}



int WriteSettingsToClient(HANDLE hProcess, uintptr_t modBaseAddress)
{
	uintptr_t entityListAddr = FindDMAAddy(hProcess, modBaseAddress + 0x10F4F8, { 0x0 });
	uintptr_t localPlayerAddr = FindDMAAddy(hProcess, modBaseAddress + 0x10F4F4, { 0x0 });
	intptr_t playerCountAddr = modBaseAddress + 0x10F500;

	Player localPlayer = LoadPlayer(hProcess, localPlayerAddr);

	int playerCount;
	ReadProcessMemory(hProcess, (BYTE*)playerCountAddr, (BYTE*)&playerCount, sizeof(playerCount), nullptr);

	auto players = LoadPlayers(hProcess, playerCount, entityListAddr);

	if (settings::godMode)
	{
		localPlayer.setArmor(hProcess, 100);
		localPlayer.setHealth(hProcess, 100);
	}

	Weapon currWeapon = LoadWeapon();

	if (settings::infiniteAmmo)
	{
		currWeapon.setAmmo(hProcess, currWeapon.getData(hProcess).magSize);
	}

	currWeapon->data->setFireCooldown(hProcess, settings::rapidFire ? 0 : 160);
	currWeapon->data->setDamage(hProcess, settings::oneTap ? 9999 : 20);

	currWeapon->data->toggleRecoil(hProcess, !settings::noRecoil);
	currWeapon->data->toggleBulletSpread(hProcess, !settings::noSpray);
	currWeapon->data->toggleAutomatic(hProcess, settings::fullAuto);
	currWeapon->data->toggleWeaponShake(hProcess, !settings::noShake);
	currWeapon->data->setReloadTime(hProcess, settings::instantReload ? 0 : 1200);
	currWeapon->data->setEnemyKnockback(hProcess, settings::knockback);

	ToggleSpeedHack(hProcess, modBaseAddress, settings::speed);
	ToggleAntiGravtiy(hProcess, modBaseAddress, settings::antiGravity);

	localPlayer.toggleGhostmode(hProcess, settings::ghostMode);
	localPlayer.toggleFlyHack(hProcess, settings::flyHack);

	localPlayer.setCurrentWeapon(hProcess, settings::selectedWeapon);
	return 1;
}

bool SetupConsole()
{
	if (!AllocConsole()) {
		MessageBox(NULL, "Failed to create a console.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	FILE* pFile;
	if (freopen_s(&pFile, "CONOUT$", "w", stdout) != 0) {
		MessageBox(NULL, "Failed to redirect stdout to console.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (freopen_s(&pFile, "CONOUT$", "w", stderr) != 0) {
		MessageBox(NULL, "Failed to redirect stderr to console.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ PSTR lpCmdLine, 
	_In_ int nCmdShow)
{
	if (!SetupConsole()) { return EXIT_FAILURE; };

	gui::CreateHWindow("Cheat Menu", "Cheat Menu Class");
	gui::CreateDevice();
	gui::CreateImGui();

	DWORD pid = 0;
	uintptr_t modBaseAddress = 0;
	HANDLE hProcess = GetProcessHandle(pid, modBaseAddress);


	while (gui::exit)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		WriteSettingsToClient(hProcess, modBaseAddress);
	}

	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	FreeConsole();
	return EXIT_SUCCESS;
};