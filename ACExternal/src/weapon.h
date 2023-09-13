#pragma once

#include "player.h"
#include "stdafx.h"

struct WeaponData
{
	char name[7]; //0x0000
	char pad_0007[257]; //0x0007
	int16_t reloadTime; //0x0108
	int16_t fireRate; //0x010A
	uint16_t damage; //0x010C
	char pad_010E[6]; //0x010E
	int16_t bulletSpread; //0x0114
	int16_t recoilBack; //0x0116
	int16_t magSize; //0x0118
	int16_t weaponShake1; //0x011A
	int16_t weaponShake2; //0x011C
	char pad_011E[4]; //0x011E
	int16_t recoilUp; //0x0122
	char pad_0124[2]; //0x0124
	int16_t enemyKnockback; //0x0126
	bool isAutomatic; //0x0128
};


struct ReserveData
{
	int32_t reserveAmmo; //0x0000
	char pad_0004[36]; //0x0004
	int32_t ammo; //0x0028
	char pad_002C[36]; //0x002C
	int32_t cooldown; //0x0050
	char pad_0054[36]; //0x0054
	int32_t totalShotsFired; //0x0078
	// char pad_007C[20]; //0x007C
};


struct Weapon
{
	~Weapon() {
		delete owner;
		delete data;
		delete reserveData;
	}

	uint32_t vTable; //0x0000
	int32_t ID; //0x0004
	struct Player* owner; //0x0008
	WeaponData* data; //0x000C
	ReserveData* reserveData; //0x0010
	// char pad_0014[4]; //0x0014

};


