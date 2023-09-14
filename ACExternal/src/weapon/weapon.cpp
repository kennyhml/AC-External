
#include "weapon.h"
#include "../mem.h"

// get size of uintptr_t so we can keep it as a buffer for the baseAddress in the objects
const int s_uintptr = sizeof(uintptr_t);

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
	ReadProcessMemory(hProcess, (BYTE*)weaponAddress, &weapon, sizeof(Weapon) - s_uintptr, nullptr);

	// The weapon.owner currently only contains the address of the owner, now we load it
	// and change the .owner pointer to the loaded object
	Player* owner = new Player((uintptr_t)weapon.owner);
	ReadProcessMemory(hProcess, (BYTE*)weapon.owner, &*owner, sizeof(Player) - s_uintptr, nullptr);
	weapon.owner = owner;

	// Do the same we did for the owner for weaponData and reserveData
	WeaponData* weaponData = new WeaponData((uintptr_t)weapon.data);
	ReadProcessMemory(hProcess, (BYTE*)weapon.data, &*weaponData, sizeof(WeaponData) - s_uintptr, nullptr);
	weapon.data = weaponData;

	ReserveData* reserveData = new ReserveData((uintptr_t)weapon.reserveData);
	ReadProcessMemory(hProcess, (BYTE*)weapon.reserveData, &*reserveData, sizeof(ReserveData) - s_uintptr, nullptr);
	weapon.reserveData = reserveData;

	return weapon;
}








