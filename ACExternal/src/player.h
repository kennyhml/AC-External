#pragma once

#include "geometry.h"
#include <string>
#include "stdafx.h"
#include <vector>

enum class MovementFlags : int32_t
{
	Forward = 1,
	Back = 255,
	Left = 256,
	Right = 65280,
	Respawn = 65536,
	NoClip = 262144,
	Ghost = 327680
};

enum class Team : int32_t
{
	CLA = 0,
	RVSF = 1,
	Spectate = 5
};

enum class Status : int32_t
{
	Alive = 0,
	Dead = 1,
	Spectate = 5
};

struct Player
{
	Player(uintptr_t baseAddress)
		:baseAddress(baseAddress) {};

	uint32_t* vTable; //0x0000
	struct Vector3 headPos; //0x0004
	struct Vector3 velocity; //0x0010
	struct IntVector3 directionalMove; //0x001C
	struct Vector3 readableSpeed; //0x0028
	struct Vector3 feedPos; //0x0034
	struct Vector3 viewAngle; //0x0040
	char pad_004C[4]; //0x004C
	float characterSpeed; //0x0050
	char pad_0054[4]; //0x0054
	struct Collider colldier; //0x0058
	int16_t isOnGround; //0x0068
	char pad_006A[6]; //0x006A
	bool isIdle; //0x0070
	bool isScoped; //0x0071
	char pad_0072[6]; //0x0072
	float zPreJump; //0x0078
	char pad_007C[4]; //0x007C
	enum MovementFlags movementFlags; //0x0080
	char pad_0084[116]; //0x0084
	int32_t health; //0x00F8
	int32_t armor; //0x00FC
	char pad_0100[40]; //0x0100
	int32_t primaryAmmoReserve; //0x0128
	char pad_012C[16]; //0x012C
	int32_t secondaryAmmo; //0x013C
	char pad_0140[12]; //0x0140
	int32_t sniperAmmo; //0x014C
	int32_t primaryAmmo; //0x0150
	char pad_0154[168]; //0x0154
	int32_t Frags; //0x01FC
	char pad_0200[4]; //0x0200
	int32_t Deaths; //0x0204
	char pad_0208[8]; //0x0208
	int32_t lastFired; //0x0210
	int32_t lastMoved; //0x0214
	char pad_0218[12]; //0x0218
	bool isShooting; //0x0224
	char name[16]; //0x0225
	char pad_0235[247]; //0x0235
	Team team; //0x032C
	char pad_0330[8]; //0x0330
	enum Status status; //0x0338
	char pad_033C[12]; //0x033C
	struct Weapon* knife; //0x0348
	struct Weapon* pistol; //0x034C
	struct Weapon* carbine; //0x0350
	struct Weapon* shotgun; //0x0354
	struct Weapon* subgun; //0x0358
	struct Weapon* sniper; //0x035C
	struct Weapon* assaultRifle; //0x0360
	struct Weapon* cPistol; //0x0364
	struct Weapon* grenade; //0x0368
	struct Weapon* akimboPistol; //0x036C
	struct Weapon* previousWeapon; //0x0370
	struct Weapon* sCurrentWeapon; //0x0374
	struct Weapon* fCurrentWeapon; //0x0378
	struct Weapon* spawnWeapon; //0x037C
	struct Weapon* nextSpawnWeapon; //0x0380
	struct Weapon* lastShotWeapon; //0x0384

	void setHealth(HANDLE hProcess, int32_t health);
	void setArmor(HANDLE hProcess, int32_t armor);
	void setTeam(HANDLE hProcess, Team team);
	void setName(HANDLE hProcess, char name[16]);

	void toggleFlyHack(HANDLE hProcess, bool flyHack);
	void toggleSpeedHack(HANDLE hProcess, bool speedHack);
	void toggleGhostmode(HANDLE hProcess, bool ghostMode);

private:
	uintptr_t baseAddress;
};


Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddress);
std::vector<Player> GetPlayers(HANDLE hProcess, int playerCount, uintptr_t entityListAddr);