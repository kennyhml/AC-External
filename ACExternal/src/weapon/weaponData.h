#pragma once

#include "..\stdafx.h"

struct WeaponData
{
	WeaponData(uintptr_t baseAddress)
		: baseAddress(baseAddress) {};

	char name[7]; //0x0000
	char pad_0007[257]; //0x0007
	int16_t reloadTime; //0x0108
	int16_t fireCooldown; //0x010A
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

	void setReloadTime(HANDLE hProcess, int16_t reloadTime);
	void setFireCooldown(HANDLE hProcess, int16_t cooldown);
	void setDamage(HANDLE hProcess, int16_t damage);
	void setMagSize(HANDLE hProcess, int16_t magSize);
	void setEnemyKnockback(HANDLE hProcess, int16_t knockback);

	void toggleBulletSpread(HANDLE hProcess, bool bulletSpread);
	void toggleRecoil(HANDLE hProcess, bool recoil);
	void toggleWeaponShake(HANDLE hProcess, bool weaponShake);
	void toggleAutomatic(HANDLE hProcess, bool automatic);

private:
	uintptr_t baseAddress;


};