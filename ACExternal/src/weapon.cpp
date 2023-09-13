
#include "weapon.h"
#include "mem.h"

template <typename T>
size_t getSize(const T& instance) { return sizeof(instance) - sizeof(uintptr_t); }


void WeaponData::setReloadTime(HANDLE hProcess, int16_t reloadTime) 
{
	PatchEx((BYTE*)(baseAddress + 0x0108), (BYTE*)&reloadTime, sizeof(reloadTime), hProcess);
}

void WeaponData::setFireRate(HANDLE hProcess, int16_t fireRate)
{

}

void WeaponData::setDamage(HANDLE hProcess, int16_t damage)
{

}

void WeaponData::setMagSize(HANDLE hProcess, int16_t magSize)
{

}

void WeaponData::toggleBulletSpread(HANDLE hProcess, bool bulletSpread)
{

}

void WeaponData::toggleRecoil(HANDLE hProcess, bool recoil)
{

}

void WeaponData::toggleWeaponShake(HANDLE hProcess, bool weaponShake)
{

}

void WeaponData::toggleAutomatic(HANDLE hProcess, bool automatic)
{

}


void ReserveData::setReserveAmmo(HANDLE hProcess, int32_t reserveAmmo)
{

}
void ReserveData::setAmmo(HANDLE hProcess, int32_t ammo)
{

}



/**
 * Loads a weapon object at the given address, including the sub structures such as:
 * - The weapons owner (Player object)
 * - The data of the weapon (weaponData object)
 * - The information about the magazine of the weapon (reserveData object)
 *
 * Note that those sub structs are allocated on the heap and the weapon object receives
 * a pointer to them, when the weapon object is destroyed, the sub structs are freed.
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
	ReadProcessMemory(hProcess, (BYTE*)weaponAddress, &weapon, getSize(weapon), nullptr);

	// The weapon.owner currently only contains the address of the owner, now we load it
	// and change the .owner pointer to the loaded object
	Player* owner = new Player;
	ReadProcessMemory(hProcess, (BYTE*)weapon.owner, &*owner, sizeof(Player), nullptr);
	weapon.owner = owner;

	// Do the same we did for the owner for weaponData and reserveData
	WeaponData* weaponData = new WeaponData((uintptr_t)weapon.data);
	ReadProcessMemory(hProcess, (BYTE*)weapon.data, &*weaponData, getSize(weaponData), nullptr);
	weapon.data = weaponData;

	ReserveData* reserveData = new ReserveData((uintptr_t)weapon.reserveData);
	ReadProcessMemory(hProcess, (BYTE*)weapon.reserveData, &*reserveData, getSize(reserveData), nullptr);
	weapon.reserveData = reserveData;

	return weapon;
}








