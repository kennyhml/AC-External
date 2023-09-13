
#include "weapon.h"
#include "../mem.h"

template <typename T>

/**
 * Gets the size of the object minus the size of a uintptr_t as we use a uintptr_t to
 * store the address of the object in memory for ease of modifying it.
 *
 * The size of a uintptr_t is platform dependent, hence this helper function.
 *
 * @param instance The instance of the object to get the size of
 *
 * @return The size of the object - the size of a uintptr_t
 */
size_t getSize(const T& instance) { return sizeof(instance) - sizeof(uintptr_t); }


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








