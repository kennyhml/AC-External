#include "weaponData.h"
#include "../mem.h"

namespace offsets {
	int name = 0x0000;
	int reloadTime = 0x0108;
	int fireCooldown = 0x010A;
	int damage = 0x010C;
	int bulletSpread = 0x0114;
	int recoilBack = 0x0116;
	int magSize = 0x0118;
	int weaponShake1 = 0x011A;
	int weaponShake2 = 0x011C;
	int recoilUp = 0x0122;
	int enemyKnockback = 0x0126;
	int isAutomatic = 0x0128;
};

/**
 * @brief Modifies the time it takes to reload this weapon, i.e the reload animation.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param reloadTime The new reload duration of the weapon (0 for instant reload).
 */
void WeaponData::setReloadTime(HANDLE hProcess, int16_t reloadTime)
{
	uintptr_t targetAddress = baseAddress + offsets::reloadTime;
	PatchEx((BYTE*)targetAddress, (BYTE*)&reloadTime, sizeof(reloadTime), hProcess);
};

/**
 * @brief Modifies the cooldown after firing a shot until the next shot can be fired.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param cooldown The new cooldown of the weapon (0 for insane rapid fire).
 */
void WeaponData::setFireCooldown(HANDLE hProcess, int16_t cooldown)
{
	uintptr_t targetAddress = baseAddress + offsets::fireCooldown;
	PatchEx((BYTE*)targetAddress, (BYTE*)&cooldown, sizeof(cooldown), hProcess);
}

/**
 * @brief Modifies the damage this weapon deals. Does not affect bot entitiy weapons.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param damage The new damage of the weapon (0 damage will prevent knockback).
 */
void WeaponData::setDamage(HANDLE hProcess, int16_t damage)
{
	uintptr_t targetAddress = baseAddress + offsets::damage;
	PatchEx((BYTE*)targetAddress, (BYTE*)&damage, sizeof(damage), hProcess);
}

/**
 * @brief Modifies the maximum ammo count that can be stored in the weapon magazine.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param magSize The new damage of the weapon (0 damage will prevent knockback).
 */
void WeaponData::setMagSize(HANDLE hProcess, int16_t magSize)
{
	uintptr_t targetAddress = baseAddress + offsets::magSize;
	PatchEx((BYTE*)targetAddress, (BYTE*)&magSize, sizeof(magSize), hProcess);

}

/**
 * @brief Modifies the knockback that is dealt when the weapon hits an enemy.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param knockback The new knockback, negative knockback will pull people towards you.
 */
void WeaponData::setEnemyKnockback(HANDLE hProcess, int16_t knockback)
{
	uintptr_t targetAddress = baseAddress + offsets::enemyKnockback;
	PatchEx((BYTE*)targetAddress, (BYTE*)&knockback, sizeof(knockback), hProcess);
}

/**
 * @brief Toggles the bulletSpread of the weapon, i.e how accurate it is.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param bulletSpread Whether to enable bullet spread or not.
 */
void WeaponData::toggleBulletSpread(HANDLE hProcess, bool bulletSpread)
{
	uintptr_t targetAddress = baseAddress + offsets::bulletSpread;

	int16_t value = bulletSpread ? 18 : 0;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Toggles the recoil of the weapon (backwards and upwards).
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param recoil Whether to enable recoil or not.
 */
void WeaponData::toggleRecoil(HANDLE hProcess, bool recoil)
{
	uintptr_t recoilBackAddr = baseAddress + offsets::recoilBack;
	uintptr_t recoilUpAddr = baseAddress + offsets::recoilUp;

	int16_t value = recoil ? 50 : 0;
	PatchEx((BYTE*)recoilBackAddr, (BYTE*)&value, sizeof(value), hProcess);
	PatchEx((BYTE*)recoilUpAddr, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Toggles the shake of the weapon while shooting.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param weaponShake Whether to enable shake animations or not.
 */
void WeaponData::toggleWeaponShake(HANDLE hProcess, bool weaponShake)
{
	uintptr_t shakeHorizonalAddr = baseAddress + offsets::weaponShake1;
	uintptr_t shakeVerticalAddr = baseAddress + offsets::weaponShake2;

	int16_t value = weaponShake ? 4 : 0;
	PatchEx((BYTE*)shakeHorizonalAddr, (BYTE*)&value, sizeof(value), hProcess);
	PatchEx((BYTE*)shakeVerticalAddr, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Toggles whether the weapon can be shot by holding left click.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param automatic Whether the weapon can be shot full-auto or not.
 */
void WeaponData::toggleAutomatic(HANDLE hProcess, bool automatic)
{
	uintptr_t targetAddress = baseAddress + offsets::isAutomatic;
	PatchEx((BYTE*)targetAddress, (BYTE*)&automatic, sizeof(automatic), hProcess);
}