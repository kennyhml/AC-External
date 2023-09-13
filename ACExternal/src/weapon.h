#pragma once

#include "player.h"
#include "stdafx.h"


struct WeaponData
{
	WeaponData(uintptr_t baseAddress)
		: baseAddress(baseAddress) {};

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

	void setReloadTime(HANDLE hProcess, int16_t reloadTime);
	void setFireRate(HANDLE hProcess, int16_t fireRate);
	void setDamage(HANDLE hProcess, int16_t damage);
	void setMagSize(HANDLE hProcess, int16_t magSize);

	void toggleBulletSpread(HANDLE hProcess, bool bulletSpread);
	void toggleRecoil(HANDLE hProcess, bool recoil);
	void toggleWeaponShake(HANDLE hProcess, bool weaponShake);
	void toggleAutomatic(HANDLE hProcess, bool automatic);

private:
	uintptr_t baseAddress;


};


struct ReserveData
{

	ReserveData(uintptr_t baseAddress)
		: baseAddress(baseAddress) {};

	int32_t reserveAmmo; //0x0000
	char pad_0004[36]; //0x0004
	int32_t ammo; //0x0028
	char pad_002C[36]; //0x002C
	int32_t cooldown; //0x0050
	char pad_0054[36]; //0x0054
	int32_t totalShotsFired; //0x0078

	void setReserveAmmo(HANDLE hProcess, int32_t reserveAmmo);
	void setAmmo(HANDLE hProcess, int32_t ammo);


private:
	uintptr_t baseAddress;

};


struct Weapon
{
	Weapon(uintptr_t baseAddress)
		: baseAddress(baseAddress) {};

	~Weapon() {
		delete owner;
		delete data;
		delete reserveData;
	}

	uint32_t vTable; //0x0000
	int32_t ID; //0x0004
	struct Player* owner; //0x0008
	struct WeaponData* data; //0x000C
	struct ReserveData* reserveData; //0x0010

private:
	uintptr_t baseAddress;
};

Weapon LoadWeapon(HANDLE hProcess, uintptr_t weaponAddress);
