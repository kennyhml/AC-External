
#include "stdafx.h"
#include "settings.h"
#include "gui.h"
#include "tools/proc.h"
#include "tools/mem.h"
#include "player/player.h"
#include "weapon/weapon.h"
#include <Windows.h>
#include <thread>
#include "esp.h"

static uintptr_t entityListAddress;
static uintptr_t localPlayerAddress;
static uintptr_t modBaseAddress;

static void CheckKeyPresses()
{
	settings::aimbot = GetAsyncKeyState(VK_CONTROL) & 0x8000;
	settings::flyHack = GetAsyncKeyState(VK_SPACE) & 0x8000;

	if (GetAsyncKeyState(VK_F1) & 1) { settings::esp = !settings::esp; }
	if (GetAsyncKeyState(VK_F2) & 1) { settings::godMode = !settings::godMode; }
	if (GetAsyncKeyState(VK_F3) & 1) { settings::speedEnabled = !settings::speedEnabled; }
	if (GetAsyncKeyState(VK_F4) & 1) { settings::noRecoil = !settings::noRecoil; }
	if (GetAsyncKeyState(VK_F5) & 1) { settings::rapidFire = !settings::rapidFire; }
	if (GetAsyncKeyState(VK_F6) & 1) { settings::infiniteAmmo = !settings::infiniteAmmo; }
	if (GetAsyncKeyState(VK_F7) & 1) { settings::noSpray = !settings::noSpray; }
	if (GetAsyncKeyState(VK_F8) & 1) { settings::flyHack = !settings::flyHack; }
	if (GetAsyncKeyState(VK_F9) & 1) { settings::ghostMode = !settings::ghostMode; }
	if (GetAsyncKeyState(VK_F10) & 1) { settings::fullAuto = !settings::fullAuto; }
}

static bool SetupConsole()
{
	if (!AllocConsole()) { return false; }
	FILE* pFile;
	return (freopen_s(&pFile, "CONOUT$", "w", stdout) != 0);
}

static HANDLE GetProcessHandle(DWORD& pid)
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

static void Aimbot(HANDLE hProcess, Player& localPlayer)
{
	int playerCount = GetPlayerCount(hProcess, modBaseAddress);
	auto players = LoadPlayers(hProcess, playerCount, entityListAddress);

	auto mode = GetGameMode(hProcess, modBaseAddress);

	float closestDiff = -1.f;
	Player closestEnemy = NULL;

	for (Player& enemy : players)
	{
		// Don't aimbot friendlies or dead players
		if (!enemy.isEnemy(localPlayer.team, mode) || !enemy.isValid()) { continue; }

		float distance = GetDistance(localPlayer.headPos, enemy.headPos);
		if (distance > settings::aimbotMaxDistance || distance < settings::aimbotMinDistance) { continue; }

		auto perfAngle = CalculateAngle(localPlayer.headPos, enemy.headPos);
		auto currAngle = localPlayer.viewAngle;

		Vector3 diff = { perfAngle.x - currAngle.x, perfAngle.z - currAngle.z, 0.f };

		if (abs(diff.x) > settings::aimbotRadius || abs(diff.y) > settings::aimbotRadius) { continue; }

		float totalDiff = abs(diff.x) + abs(diff.y);
		if (totalDiff < closestDiff || closestDiff == -1)
		{
			closestDiff = totalDiff;
			closestEnemy = enemy;
		}
	}

	if (closestDiff == -1) { return; }
	std::cout << "[+] Closest enemy to aimbot: " << closestEnemy.name << "\n!";

	Vector3 angle = CalculateAngle(localPlayer.headPos, closestEnemy.headPos);
	localPlayer.setView(hProcess, angle);
}

static void WriteSettingsToClient(HANDLE hProcess)
{
	Player localPlayer = LoadPlayer(hProcess, localPlayerAddress);

	if (settings::godMode)
	{
		localPlayer.setArmor(hProcess, 999);
		localPlayer.setHealth(hProcess, 999);
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

	if (settings::aimbot) {
		Aimbot(hProcess, localPlayer);
	}
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR lpCmdLine,
	_In_ int nCmdShow)
{
	if (!SetupConsole()) { return EXIT_FAILURE; };

	DWORD pid = 0;
	HANDLE hProcess = GetProcessHandle(pid);
	entityListAddress = GetPointedAddress(hProcess, modBaseAddress + 0x10F4F8);
	localPlayerAddress = GetPointedAddress(hProcess, modBaseAddress + 0x10F4F4);

	std::thread espThread(ESP::Run, hProcess, modBaseAddress, localPlayerAddress, entityListAddress, std::ref(gui::exit));
	espThread.detach();

	gui::CreateHWindow("Cheat Menu", "Cheat Menu Class");
	gui::CreateDevice();
	gui::CreateImGui();

	while (gui::exit)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		CheckKeyPresses();
		WriteSettingsToClient(hProcess);
	}
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	FreeConsole();
	return EXIT_SUCCESS;
};