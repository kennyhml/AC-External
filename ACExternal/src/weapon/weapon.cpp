
#include "weapon.h"
#include "../mem.h"

/**
 * Loads a weapon object from the given address in the target process.
 *
 * @param hProcess The handle to the process to read memory from.
 * @param weaponAddress The direct address of the weapon object.
 *
 * @return A reconstruction of the weapon object located at the given address.
 */
Weapon LoadWeapon(HANDLE hProcess, uintptr_t weaponAddress)
{
	// Load the weapon first, this will provide us with the pointers to the weapon
	// owner, the weapon data and the reserve (magazine) data.
	Weapon weapon(weaponAddress);

	int size = sizeof(ReserveData) - sizeof(uintptr_t);
	ReadProcessMemory(hProcess, (BYTE*)weaponAddress, &weapon, size, nullptr);

	return weapon;
}

/**
 * Loads a WeaponData object pointed to by this Weapon object.
 *
 * @param hProcess The handle to the process to read memory from.
 *
 * @return The WeaponData object of this Weapon.
 */
WeaponData Weapon::getData(HANDLE hProcess)
{
	WeaponData data(dataPointer);

	int size = sizeof(WeaponData) - sizeof(uintptr_t);
	ReadProcessMemory(hProcess, (BYTE*)dataPointer, &data, size, nullptr);
	return data;
}

/**
 * Loads a ReserveData object pointed to by this Weapon object.
 *
 * @param hProcess The handle to the process to read memory from.
 *
 * @return The ReserveData object of this Weapon.
 */
ReserveData Weapon::getReserveData(HANDLE hProcess)
{
	ReserveData reserveData(reserveDataPointer);

	int size = sizeof(ReserveData) - sizeof(uintptr_t);
	ReadProcessMemory(hProcess, (BYTE*)reserveDataPointer, &reserveData, size, nullptr);
	return reserveData;
}

/**
 * @brief Modifies the time it takes to reload this weapon, i.e the reload animation.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param reloadTime The new reload duration of the weapon (0 for instant reload).
 */
void Weapon::setReloadTime(HANDLE hProcess, int16_t reloadTime)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x108 });
	PatchEx((BYTE*)targetAddress, (BYTE*)&reloadTime, sizeof(reloadTime), hProcess);
};

/**
 * @brief Modifies the cooldown after firing a shot until the next shot can be fired.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param cooldown The new cooldown of the weapon (0 for insane rapid fire).
 */
void Weapon::setFireCooldown(HANDLE hProcess, int16_t cooldown)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x10A });
	PatchEx((BYTE*)targetAddress, (BYTE*)&cooldown, sizeof(cooldown), hProcess);
}

/**
 * @brief Modifies the damage this weapon deals.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param damage The new damage of the weapon (0 damage will prevent knockback).
 */
void Weapon::setDamage(HANDLE hProcess, int16_t damage)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x10C });
	PatchEx((BYTE*)targetAddress, (BYTE*)&damage, sizeof(damage), hProcess);
}

/**
 * @brief Modifies the maximum ammo count that can be stored in the weapon magazine.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param magSize The new damage of the weapon (0 damage will prevent knockback).
 */
void Weapon::setMagSize(HANDLE hProcess, int16_t magSize)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x118 });
	PatchEx((BYTE*)targetAddress, (BYTE*)&magSize, sizeof(magSize), hProcess);
}

/**
 * @brief Modifies the knockback that is dealt when the weapon hits an enemy.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param knockback The new knockback, negative knockback will pull people towards you.
 */
void Weapon::setEnemyKnockback(HANDLE hProcess, int16_t knockback)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x0126 });
	PatchEx((BYTE*)targetAddress, (BYTE*)&knockback, sizeof(knockback), hProcess);
}

/**
 * @brief Toggles the bulletSpread of the weapon, i.e how accurate it is.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param bulletSpread Whether to enable bullet spread or not.
 */
void Weapon::toggleBulletSpread(HANDLE hProcess, bool bulletSpread)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x114 });
	int16_t value = bulletSpread ? 18 : 0;
	PatchEx((BYTE*)targetAddress, (BYTE*)&value, sizeof(value), hProcess);
}

/**
 * @brief Toggles the recoil of the weapon (backwards and upwards).
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param recoil Whether to enable recoil or not.
 */
void Weapon::toggleRecoil(HANDLE hProcess, bool recoil)
{
	uintptr_t recoilBackAddr = FindDMAAddy(hProcess, dataPointer, { 0x116 });
	uintptr_t recoilUpAddr = FindDMAAddy(hProcess, dataPointer, { 0x122 });

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
void Weapon::toggleWeaponShake(HANDLE hProcess, bool weaponShake)
{
	uintptr_t shakeHorizonalAddr = FindDMAAddy(hProcess, dataPointer, { 0x011A });
	uintptr_t shakeVerticalAddr = FindDMAAddy(hProcess, dataPointer, { 0x011C });
	
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
void Weapon::toggleAutomatic(HANDLE hProcess, bool automatic)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, dataPointer, { 0x0128 });
	PatchEx((BYTE*)targetAddress, (BYTE*)&automatic, sizeof(automatic), hProcess);
}

/**
 * @brief Modifies the ammo count in the reserve magazine of the weapon to the specified value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param reserveAmmo The new ammo count to set in the reserve magazine.
 */
void Weapon::setReserveAmmo(HANDLE hProcess, int32_t reserveAmmo)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, reserveDataPointer, { 0x0 });
	PatchEx((BYTE*)targetAddress, (BYTE*)&reserveAmmo, sizeof(reserveAmmo), hProcess);
}

/**
 * @brief Modifies the ammo count in the magazine of the weapon to the specified value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param ammo The new ammo count to set in the magazine.
 */
void Weapon::setAmmo(HANDLE hProcess, int32_t ammo)
{
	uintptr_t targetAddress = FindDMAAddy(hProcess, reserveDataPointer, { 0x28 });
	PatchEx((BYTE*)targetAddress, (BYTE*)&ammo, sizeof(ammo), hProcess);

}

