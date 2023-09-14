#include "player.h"
#include "mem.h"

namespace offsets
{
	int health = 0xF8;
	int armor = 0xFC;
	int team = 0x32C;
	int moveFlag = 0x80;
	int nameO = 0x225;
	int status = 0x338;
}

Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddress)
{
	Player player(playerAddress);
	ReadProcessMemory(hProcess, (BYTE*)playerAddress, &player, sizeof(Player) - sizeof(uintptr_t), nullptr);
	return player;
};

std::vector<Player> GetPlayers(HANDLE hProcess, int entityCount, uintptr_t entityListAddr)
{
	std::vector<Player> players;

	for (int i = 1; i < entityCount; i++)
	{
		int offset = 4 * i;
		uintptr_t playerAddress;
		ReadProcessMemory(hProcess, (BYTE*)entityListAddr + offset, &playerAddress, sizeof(playerAddress), nullptr);
		Player player = LoadPlayer(hProcess, playerAddress);
		players.push_back(player);
	}

	return players;
}

void Player::setHealth(HANDLE hProcess, int32_t health)
{
	uintptr_t targetAddress = baseAddress + offsets::health;
	PatchEx((BYTE*)targetAddress, (BYTE*)&health, sizeof(health), hProcess);
};

void Player::setArmor(HANDLE hProcess, int32_t armor)
{
	uintptr_t targetAddress = baseAddress + offsets::armor;
	PatchEx((BYTE*)targetAddress, (BYTE*)&armor, sizeof(armor), hProcess);
};

void Player::setTeam(HANDLE hProcess, Team team)
{
	uintptr_t targetAddress = baseAddress + offsets::team;
	PatchEx((BYTE*)targetAddress, (BYTE*)&team, sizeof(team), hProcess);
}

void Player::setName(HANDLE hProcess, char name[16])
{
	uintptr_t targetAddress = baseAddress + offsets::nameO;
	PatchEx((BYTE*)targetAddress, (BYTE*)name, strlen(name), hProcess);
}

void Player::toggleFlyHack(HANDLE hProcess, bool flyHack)
{

}

void Player::toggleSpeedHack(HANDLE hProcess, bool speedHack)
{

}

void Player::toggleGhostmode(HANDLE hProcess, bool ghostMode)
{

}