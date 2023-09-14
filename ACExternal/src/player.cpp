#include "player.h"
#include "mem.h"
#include "weapon/weapon.h"
#include <array>

namespace offsets
{
	int health = 0xF8;
	int armor = 0xFC;
	int team = 0x32C;
	int moveFlag = 0x80;
	int nameO = 0x225;
	int status = 0x338;
	int speedW = 0x5BEA1; // positive forwards
	int speedA = 0x5BF01; // positive sidewards
	int speedS = 0x5BE41; // negative forwards
	int speedD = 0x5BF61; // negative sidewards

}

/**
 * @brief Loads a player at a given memory address, including it's weapons.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param playerAddress The address of the player to load.
 */
Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddress)
{
	Player player(playerAddress);
	ReadProcessMemory(hProcess, (BYTE*)playerAddress, &player, sizeof(Player) - sizeof(uintptr_t), nullptr);

	// Collect the pointers to all our weapon pointers in this player, so that we can later
	// change the value to point to a proper loaded weapon object on the heap
	std::array<Weapon**, 16> playerWeapons = {
		&player.knife,
		&player.pistol,
		&player.carbine,
		&player.shotgun,
		&player.subgun,
		&player.sniper,
		&player.assaultRifle,
		&player.cPistol,
		&player.grenade,
		&player.akimboPistol,
		&player.previousWeapon,
		&player.sCurrentWeapon,
		&player.fCurrentWeapon,
		&player.spawnWeapon,
		&player.nextSpawnWeapon,
		&player.lastShotWeapon
	};

	// Create all the weapon objects. The reason I want to load all of them is because it seems that
	// not every entity has a unique weapon pointer assigned, including the local player.
	// Meaning if we modify the stats of our local player weapon, we will also possibly modify the gun
	// of some other entities.
	// To ensure this doesnt happen, after loading all entities, I will make sure that all of them 
	// share the same weapon pointer and our player gets assigned a unique pointer.
	// That way we can safely make our weapon op or make the enemies weapons useless without cross backfiring.
	for (Weapon** weaponPtr : playerWeapons) {
		*weaponPtr = new Weapon(LoadWeapon(hProcess, (uintptr_t)*weaponPtr));
	}

	return player;
};

/**
 * @brief Loads all players in the entity list, does not include the local player.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param entityCount The amount of entities in the game, includes the local player.
 * @param entityListAddr The memory address of the entity list to load the players from.
 */
std::vector<Player> LoadPlayers(HANDLE hProcess, int entityCount, uintptr_t entityListAddr)
{
	std::vector<Player> players;

	for (int i = 1; i < entityCount; i++)
	{
		int offset = 4 * i;
		uintptr_t playerAddress = FindDMAAddy(hProcess, entityListAddr + offset, { 0x0 });
		Player player = LoadPlayer(hProcess, playerAddress);
		players.push_back(player);
	}

	return players;
}

/**
 * @brief Sets the health of this player to a given value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param health The value to set the this players health to.
 */
void Player::setHealth(HANDLE hProcess, int32_t health)
{
	uintptr_t targetAddress = baseAddress + offsets::health;
	PatchEx((BYTE*)targetAddress, (BYTE*)&health, sizeof(health), hProcess);
};

/**
 * @brief Sets the armor of this player to a given value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param armor The value to set the this players armor to.
 */
void Player::setArmor(HANDLE hProcess, int32_t armor)
{
	uintptr_t targetAddress = baseAddress + offsets::armor;
	PatchEx((BYTE*)targetAddress, (BYTE*)&armor, sizeof(armor), hProcess);
};

/**
 * @brief Switches this player to the given team.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param Team The team to set this player to.
 */
void Player::setTeam(HANDLE hProcess, Team team)
{
	uintptr_t targetAddress = baseAddress + offsets::team;
	PatchEx((BYTE*)targetAddress, (BYTE*)&team, sizeof(team), hProcess);
}

/**
 * @brief Changes the players name to a given value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param name The name to give the player, max 16 bytes.
 */
void Player::setName(HANDLE hProcess, char name[16])
{
	uintptr_t targetAddress = baseAddress + offsets::nameO;
	PatchEx((BYTE*)targetAddress, (BYTE*)name, strlen(name), hProcess);
}

/**
 * @brief Enable/disable this player to fly.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param flyHack Whether the player is flying or not.
 */
void Player::toggleFlyHack(HANDLE hProcess, bool flyHack)
{
	uintptr_t targetAddress = baseAddress + offsets::status;
	int value = flyHack ? (int)Status::Spectate : (int)Status::Alive;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Enable/disable ghost mode / no clip on this player.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param ghostMode Whether the player should be in ghost mode or not.
 */
void Player::toggleGhostmode(HANDLE hProcess, bool ghostMode)
{
	uintptr_t targetAddress = baseAddress + offsets::moveFlag;
	int value = ghostMode ? (int)MovementFlags::NoClip : (int)MovementFlags::None;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Enable/disable speed hack for the local player.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param modBaseAddress The base address of the module.
 * @param speedHack Whether speed hack should be on or not.
 */
void ToggleSpeedHack(HANDLE hProcess, uintptr_t modBaseAddress, bool speedHack)
{
	int32_t speed = speedHack ? 3 : 1;
	PatchEx((BYTE*)modBaseAddress + offsets::speedW, (BYTE*)&speed, 4, hProcess);
	PatchEx((BYTE*)modBaseAddress + offsets::speedA, (BYTE*)&speed, 4, hProcess);

	speed = speed * -1;
	PatchEx((BYTE*)modBaseAddress + offsets::speedS, (BYTE*)&speed, 4, hProcess);
	PatchEx((BYTE*)modBaseAddress + offsets::speedD, (BYTE*)&speed, 4, hProcess);
}

/**
 * @brief Enable/disable anti gravity for all players.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param modBaseAddress The base address of the module.
 * @param antiGravity Whether anti gravity should be on or not.
 */
void ToggleAntiGravtiy(HANDLE hProcess, uintptr_t modBaseAddress, bool antiGravity)
{
	uintptr_t instructionAddress = modBaseAddress + 0x5B285;
	if (antiGravity)
	{
		// Write NOPs into the address to prevent the fall speed from increasing
		NopEx((BYTE*)instructionAddress, 3, hProcess);
	}
	else
	{	
		// Write 'add [ebx+54],exc' back into the instruction address
		PatchEx((BYTE*)instructionAddress, (BYTE*)"\x01\x4B\x54", 3, hProcess);
	}
};

