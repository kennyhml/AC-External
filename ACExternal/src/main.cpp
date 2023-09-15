
#include "stdafx.h"
#include <iostream> 
#include <vector>
#include <Windows.h>
#include <cstdio>

#include "settings.h"
#include "gui.h"
#include "proc.h"
#include "player.h"
#include "weapon/weapon.h"

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

int WriteSettingsToClient(HANDLE hProcess, uintptr_t modBaseAddress)
{
	uintptr_t entityListAddr = FindDMAAddy(hProcess, modBaseAddress + 0x10F4F8, { 0x0 });
	uintptr_t localPlayerAddr = FindDMAAddy(hProcess, modBaseAddress + 0x10F4F4, { 0x0 });
	intptr_t playerCountAddr = modBaseAddress + 0x10F500;

	Player localPlayer = LoadPlayer(hProcess, localPlayerAddr);

	if (settings::godMode)
	{
		localPlayer.setArmor(hProcess, 100);
		localPlayer.setHealth(hProcess, 100);
	}

	Weapon* currWeapon = localPlayer.sCurrentWeapon;

	if (settings::infiniteAmmo)
	{
		currWeapon->reserveData->setAmmo(hProcess, currWeapon->data->magSize);
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

	return 0;

	int playerCount;
	ReadProcessMemory(hProcess, (BYTE*)playerCountAddr, (BYTE*)&playerCount, sizeof(playerCount), nullptr);

	std::cout << "Player count: " << playerCount << "\n";
	auto players = LoadPlayers(hProcess, playerCount, entityListAddr);

	for (Player p : players)
	{
		std::cout << p.name << "\n";

	}

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