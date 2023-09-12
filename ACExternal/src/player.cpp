#include "player.h"
#include "stdafx.h"

namespace offsets {

	// head view vector of entity
	int headX = 0x4;
	int headY = 0x8;
	int headZ = 0xC;

	// body view vector of entity
	int bodyX = 0x34;
	int bodyY = 0x38;
	int bodyZ = 0x3C;

	// view vector of entity (up and side axis)
	int viewX = 0x40;
	int viewY = 0x44;

	// Nothing really interesting between

	// Health & armor
	int health = 0xF8;
	int armor = 0xFC;

	// Entity name, 16 bytes
	int name = 0x225;

	// This one might be local player only? Doesnt seem to work on botents.
	int currentWeapon = 0x374;

	// Careful, shooting friends makes it go negative so its signed
	int frags = 0x1FC;
	int deaths = 0x204;
	int shooting = 0x224;

	// Team 0 = CLA, 1 = RVSF
	int team = 0x32C;

	// status 0: alive 1: dead 5: spectate
	int status = 0x338;


	// weapon pointers
	int knife = 0x348;
	int pistol = 0x34C;
	int carbine = 0x350;
	int shotgun = 0xd354;
	int subgun = 0x358;
	int sniper = 0x35C;
	int assaultRifle = 0x360;
	int CPistol = 0x364;
	int grenade = 0x368;
	int akimboPistol = 0x36c; // double pistol shit
	int previousWeapon = 0x370;
	int currentWeapon = 0x374;

}


void Player::getHealth(HANDLE hProcess)
{
	uintptr_t targetAddr = baseAddress + offsets::health;
	ReadProcessMemory(hProcess, (BYTE*)targetAddr, &health, sizeof(health), nullptr);
}
	
void Player::getArmor(HANDLE hProcess)
{
	uintptr_t targetAddr = baseAddress + offsets::armor;
	ReadProcessMemory(hProcess, (BYTE*)targetAddr, &armor, sizeof(armor), nullptr);

}

void Player::getDistance(Player localPlayer)
{

}


void Player::getAngleTo(Player localPlayer)
{

}

void Player::getTeam(HANDLE hProcess)
{

}

void Player::getState(HANDLE hProcess)
{

}

void Player::getAngle(HANDLE hProcess)
{
	float viewX, viewY;
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::viewX, &viewX, sizeof(viewX), nullptr);
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::viewY, &viewY, sizeof(viewY), nullptr);

	angle = vec2{ viewX, viewY };
};

void Player::getName(HANDLE hProcess)
{
	uintptr_t targetAddr = baseAddress + offsets::name;
	char buffer[16];

	ReadProcessMemory(hProcess, (BYTE*)targetAddr, &buffer, sizeof(buffer), nullptr);
	name = std::string(buffer, sizeof(buffer));
}

void Player::getPosition(HANDLE hProcess)
{
	float hX, hY, hZ;
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::headX, &hX, sizeof(hX), nullptr);
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::headY, &hY, sizeof(hY), nullptr);
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::headZ, &hZ, sizeof(hZ), nullptr);
	headPosition = vec3{ hX, hY, hZ };

	float bX, bY, bZ;
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::bodyX, &bX, sizeof(bX), nullptr);
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::bodyY, &bY, sizeof(bY), nullptr);
	ReadProcessMemory(hProcess, (BYTE*)baseAddress + offsets::bodyZ, &bZ, sizeof(bZ), nullptr);
	bodyPosition = vec3{ bX, bY, bZ };

}




bool Player::isValid() { return health > 0 && state == ALIVE; };

bool Player::isVisible() { return true; };






