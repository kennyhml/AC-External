#include "player.h"
#include "../tools/mem.h"
#include "../weapon/weapon.h"

/**
 * @brief Loads a player at a given memory address, including it's weapons.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param playerAddress The address of the player to load.
 */
Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddress)
{
	Player player(playerAddress);
	int size = sizeof(Player) - sizeof(uintptr_t);
	ReadProcessMemory(hProcess, (BYTE*)playerAddress, &player, size, nullptr);
	return player;
};

/**
 * @brief Loads all players in the entity list, does not include the local player.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param entityCount The amount of entities in the game, includes the local player.
 * @param entityListAddr The memory address of the entity list to load the players from.
 */
std::vector<Player> LoadPlayers(HANDLE hProcess, int count, uintptr_t entityListAddr)
{
	std::vector<Player> players;
	players.reserve(count);

	for (int i = 1; i < count; i++)
	{
		int offset = 4 * i;
		uintptr_t playerAddress = FindDMAAddy(hProcess, entityListAddr + offset, { 0x0 });
		players.push_back(LoadPlayer(hProcess, playerAddress));
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
	uintptr_t targetAddress = baseAddress + 0xF8;
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
	uintptr_t targetAddress = baseAddress + 0xFC;
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
	uintptr_t targetAddress = baseAddress + 0x32C;
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
	uintptr_t targetAddress = baseAddress + 0x225;
	PatchEx((BYTE*)targetAddress, (BYTE*)name, strlen(name), hProcess);
}

void Player::setCurrentWeapon(HANDLE hProcess, const char* weapon)
{
	if (weapon == NULL) { return; }
	uintptr_t weaponPointer;

	if (!strcmp(weapon, "Assault Rifle")) { weaponPointer = assaultRiflePointer; }
	else if (!strcmp(weapon, "Sniper Rifle")) { weaponPointer = sniperPointer; }
	else if (!strcmp(weapon, "Shotgun")) { weaponPointer = shotgunPointer; }
	else if (!strcmp(weapon, "Grenade")) { weaponPointer = grenadePointer; }
	else if (!strcmp(weapon, "Carbine")) { weaponPointer = carbinePointer; }
	else if (!strcmp(weapon, "Akimbo")) { weaponPointer = akimboPistolPointer; }
	else if (!strcmp(weapon, "Knife")) { weaponPointer = knifePointer; }

	else {
		throw std::runtime_error("Unknown weapon!");
	}

	if (weaponPointer == currentWeaponPointer) { return; }

	previousWeaponPointer = currentWeaponPointer;
	currentWeaponPointer = weaponPointer;

	uintptr_t targetAddress = baseAddress + 0x0374;
	PatchEx((BYTE*)targetAddress, (BYTE*)&currentWeaponPointer, sizeof(uintptr_t), hProcess);
	std::cout << "[+] Gun has been switched to " << weapon << "!\n";
}

/**
 * @brief Changes the player position to a given vec3.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param position The new position of the player.
 */
void Player::setPosition(HANDLE hProcess, Vector3 position)
{
	uintptr_t targetAddress = baseAddress + 0x34;
	PatchEx((BYTE*)targetAddress, (BYTE*)&position, sizeof(position), hProcess);
}

/**
 * @brief Changes the player view angles to a given vec3.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param view The new view of the player.
 */
void Player::setView(HANDLE hProcess, Vector3& view)
{

	uintptr_t targetAddress = baseAddress + 0x40;
	PatchEx((BYTE*)targetAddress, (BYTE*)&view, sizeof(view), hProcess);
}

/**
 * @brief Enable/disable player to be using his weapon.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param attacking Whether the player should be attacking or not.
 */
void Player::toggleAttacking(HANDLE hProcess, bool attacking)
{
	uintptr_t targetAddress = baseAddress + 0x224;
	int value = attacking ? 1 : 0;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Enable/disable this player to fly.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param flyHack Whether the player is flying or not.
 */
void Player::toggleFlyHack(HANDLE hProcess, bool flyHack)
{
	if (flyHack == (status == Status::Spectate)) { return; }

	uintptr_t targetAddress = baseAddress + 0x338;
	int value = flyHack ? (int)Status::Spectate : (int)Status::Alive;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);

	std::cout << (flyHack ? "[+] Fly activated!\n" : "[+] Fly deactivated!\n");
}

/**
 * @brief Enable/disable ghost mode / no clip on this player.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param ghostMode Whether the player should be in ghost mode or not.
 */
void Player::toggleGhostmode(HANDLE hProcess, bool ghostMode)
{

	if ((int)movementFlag & (int)MovementFlags::NoClip && ghostMode) return;
	if (!((int)movementFlag & (int)MovementFlags::NoClip) && !ghostMode) return;

	uintptr_t targetAddress = baseAddress + 0x80;
	int value = ghostMode ? (int)movementFlag + (int)MovementFlags::NoClip : 0;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);

	std::cout << (ghostMode ? "[+] Ghost mode activated!\n" : "[+] Ghost mode deactivated!\n");
}

/**
 * @brief Enable/disable speed hack for the local player.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param modBaseAddress The base address of the module.
 * @param speedHack Whether speed hack should be on or not.
 */
void ToggleSpeedHack(HANDLE hProcess, uintptr_t modBaseAddress, uint8_t speed)
{

	PatchEx((BYTE*)modBaseAddress + 0x5BEA1, (BYTE*)&speed, 4, hProcess);
	PatchEx((BYTE*)modBaseAddress + 0x5BF01, (BYTE*)&speed, 4, hProcess);

	speed = speed * -1;
	PatchEx((BYTE*)modBaseAddress + 0x5BE41, (BYTE*)&speed, 4, hProcess);
	PatchEx((BYTE*)modBaseAddress + 0x5BF61, (BYTE*)&speed, 4, hProcess);
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


bool Player::isEnemy(Team localPlayerTeam, GameMode mode) const
{
	return (
		team != localPlayerTeam
		|| mode == GameMode::BotDeathmatch || mode == GameMode::Deathmatch
		|| mode == GameMode::BotOneShotOneKill || mode == GameMode::OneShotOneKill
		|| mode == GameMode::BotPistolFrenz || mode == GameMode::PistolFrenzy
		|| mode == GameMode::BotLastSwissStanding || mode == GameMode::LastSwissStanding);
}

bool Player::isValid()
{
	return status == Status::Alive && health > 0 && health < 9999;
}

int GetPlayerCount(HANDLE hProcess, uintptr_t modBaseAddress)
{
	int count;
	uintptr_t targetAddress = modBaseAddress + 0x10F500;
	ReadProcessMemory(hProcess, (BYTE*)targetAddress, &count, sizeof(count), nullptr);
	return count;
}

GameMode GetGameMode(HANDLE hProcess, uintptr_t modBaseAddress)
{
	int mode;
	uintptr_t targetAddress = modBaseAddress + 0x10F49C;
	ReadProcessMemory(hProcess, (BYTE*)targetAddress, &mode, sizeof(mode), nullptr);
	return GameMode(mode);
}