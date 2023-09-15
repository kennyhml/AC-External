#pragma once

#include "geometry.h"
#include <string>
#include "../stdafx.h"
#include "../weapon/weapon.h"
#include "enums.h"

struct Player
{
	Player(uintptr_t baseAddress)
		:baseAddress(baseAddress) {};

	uint32_t vTable; //0x0000
	Vector3 headPos; //0x0004
	Vector3 velocity; //0x0010
	IntVector3 directionalMove; //0x001C
	Vector3 readableSpeed; //0x0028
	Vector3 feetPos; //0x0034
	Vector3 viewAngle; //0x0040
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
	enum MovementFlags movementFlag; //0x0080
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
	int32_t knifePointer; //0x0348
	int32_t pistolPointer; //0x034C
	int32_t carbinePointer; //0x0350
	int32_t shotgunPointer; //0x0354
	int32_t subgunPointer; //0x0358
	int32_t sniperPointer; //0x035C
	int32_t assaultRiflePointer; //0x0360
	int32_t cPistolPointer; //0x0364
	int32_t grenadePointer; //0x0368
	int32_t akimboPistolPointer; //0x036C
	int32_t previousWeaponPointer; //0x0370
	int32_t currentWeaponPointer; //0x0374
	int32_t ignoreThisPointer; //0x0378
	int32_t spawnWeaponPointer; //0x037C
	int32_t nextSpawnWeaponPointer; //0x0380
	int32_t lastShotWeaponPointer; //0x0384
	uintptr_t baseAddress;

	void setHealth(HANDLE hProcess, int32_t health);
	void setArmor(HANDLE hProcess, int32_t armor);
	void setTeam(HANDLE hProcess, Team team);
	void setName(HANDLE hProcess, char name[16]);
	void setCurrentWeapon(HANDLE hProcess, const char* weapon);

	void toggleFlyHack(HANDLE hProcess, bool flyHack);
	void toggleGhostmode(HANDLE hProcess, bool ghostMode);
};


Player LoadPlayer(HANDLE hProcess, uintptr_t playerAddress);
std::vector<Player> LoadPlayers(HANDLE hProcess, int playerCount, uintptr_t entityListAddr);
void ToggleAntiGravtiy(HANDLE hProcess, uintptr_t modBaseAddress, bool antiGravity);
void ToggleSpeedHack(HANDLE hProcess, uintptr_t modBaseAddress, uint8_t speed);