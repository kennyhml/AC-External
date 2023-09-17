
#include "stdafx.h"
#include "settings.h"
#include "gui.h"
#include "proc.h"
#include "player/player.h"
#include "weapon/weapon.h"
#include "mem.h"

#include <Windows.h>



static uintptr_t entityListAddress;
static uintptr_t localPlayerAddress;
static uintptr_t playerCountAddress;


static HANDLE GetProcessHandle(DWORD& pid, uintptr_t& modBaseAddress)
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

static void InstaKillEveryone(HANDLE hProcess, Player localPlayer, std::vector<Player> players)
{

	localPlayer.setHealth(hProcess, 9999);
	localPlayer.setCurrentWeapon(hProcess, "Knife");
	// localPlayer.setView(hProcess, Vector3(localPlayer.viewAngle.x, 90, 0));
	Weapon knife = LoadWeapon(hProcess, localPlayer.currentWeaponPointer);

	knife.setDamage(hProcess, 200);
	knife.setFireCooldown(hProcess, 0);

	for (Player& targetPlayer : players) {
		targetPlayer = LoadPlayer(hProcess, targetPlayer.baseAddress);

		if (targetPlayer.team == localPlayer.team || 
			targetPlayer.status != Status::Alive ) { continue; }

		while (targetPlayer.status == Status::Alive) {
			localPlayer.setHealth(hProcess, 9999);
			localPlayer.setPosition(hProcess, targetPlayer.feetPos);
			localPlayer.toggleAttacking(hProcess, true);
			targetPlayer = LoadPlayer(hProcess, targetPlayer.baseAddress);
		}
		std::cout << "[+] Slaughtered " << targetPlayer.name << "!\n";
	}

	localPlayer.toggleAttacking(hProcess, false);
	knife.setDamage(hProcess, 20);
	knife.setFireCooldown(hProcess, 120);
}

static void WriteSettingsToClient(HANDLE hProcess, uintptr_t modBaseAddress)
{
	int playerCount = GetPlayerCount(hProcess, modBaseAddress);
	Player localPlayer = LoadPlayer(hProcess, localPlayerAddress);
	auto players = LoadPlayers(hProcess, playerCount, entityListAddress);

	Player target = LoadPlayer(hProcess, players.at(2).baseAddress);

	Vector3 angle = CalculateAngles(localPlayer.headPos, target.headPos);


	localPlayer.setView(hProcess, angle);
	return;

	if (settings::godMode)
	{
		localPlayer.setArmor(hProcess, 100);
		localPlayer.setHealth(hProcess, 100);
	}


	Weapon currWeapon = LoadWeapon(hProcess, localPlayer.currentWeaponPointer);
	if (settings::infiniteAmmo)
	{
		currWeapon.setAmmo(hProcess, currWeapon.getData(hProcess).magSize);
	}


	currWeapon.setFireCooldown(hProcess, settings::rapidFire ? 0 : 160);
	currWeapon.setDamage(hProcess, settings::oneTap ? 9999 : 20);
	currWeapon.toggleRecoil(hProcess, !settings::noRecoil);
	currWeapon.toggleBulletSpread(hProcess, !settings::noSpray);
	currWeapon.toggleAutomatic(hProcess, settings::fullAuto);
	currWeapon.toggleWeaponShake(hProcess, !settings::noShake);
	currWeapon.setReloadTime(hProcess, settings::instantReload ? 0 : 1200);
	currWeapon.setEnemyKnockback(hProcess, settings::knockback);

	ToggleSpeedHack(hProcess, modBaseAddress, settings::speed);
	ToggleAntiGravtiy(hProcess, modBaseAddress, settings::antiGravity);

	localPlayer.toggleGhostmode(hProcess, settings::ghostMode);
	localPlayer.toggleFlyHack(hProcess, settings::flyHack);

	localPlayer.setCurrentWeapon(hProcess, settings::selectedWeapon);
}

static bool SetupConsole()
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
	entityListAddress = GetPointedAddress(hProcess, modBaseAddress + 0x10F4F8);
	localPlayerAddress = GetPointedAddress(hProcess, modBaseAddress + 0x10F4F4);
	playerCountAddress = GetPointedAddress(hProcess, modBaseAddress + 0x10F500);

	Sleep(3000);

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