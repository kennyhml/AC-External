#pragma once
#include "weaponData.h"
#include "reserveData.h"

struct Weapon
{
	Weapon(uintptr_t baseAddress)
		: baseAddress(baseAddress) {};


	uint32_t vTable; //0x0000
	int32_t ID; //0x0004
	int32_t owner; //0x0008
	int32_t dataPointer; //0x000C
	int32_t reserveDataPointer; //0x0010
	uintptr_t baseAddress;

	WeaponData getData(HANDLE hProcess);
	ReserveData getReserveData(HANDLE hProcess);

	void setReloadTime(HANDLE hProcess, int16_t reloadTime);
	void setFireCooldown(HANDLE hProcess, int16_t cooldown);
	void setDamage(HANDLE hProcess, int16_t damage);
	void setMagSize(HANDLE hProcess, int16_t magSize);
	void setEnemyKnockback(HANDLE hProcess, int16_t knockback);
	void setReserveAmmo(HANDLE hProcess, int32_t reserveAmmo);
	void setAmmo(HANDLE hProcess, int32_t ammo);

	void toggleBulletSpread(HANDLE hProcess, bool bulletSpread);
	void toggleRecoil(HANDLE hProcess, bool recoil);
	void toggleWeaponShake(HANDLE hProcess, bool weaponShake);
	void toggleAutomatic(HANDLE hProcess, bool automatic);
};

Weapon LoadWeapon(HANDLE hProcess, uintptr_t weaponAddress);
