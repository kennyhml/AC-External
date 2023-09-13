#include "reserveData.h"
#include "../mem.h"


namespace offsets {
	int reserveAmmo = 0x0000;
	int ammo = 0x0028;
	int cooldown = 0x0050;
	int totalShotsFired = 0x0078;
};


/**
 * @brief Modifies the ammo count in the reserve magazine of the weapon to the specified value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param reserveAmmo The new ammo count to set in the reserve magazine.
 */
void ReserveData::setReserveAmmo(HANDLE hProcess, int32_t reserveAmmo)
{
	uintptr_t targetAddress = baseAddress + offsets::reserveAmmo;
	PatchEx((BYTE*)targetAddress, (BYTE*)&reserveAmmo, sizeof(reserveAmmo), hProcess);
}

/**
 * @brief Modifies the ammo count in the magazine of the weapon to the specified value.
 *
 * @param hProcess A handle to the target process for memory modification.
 * @param ammo The new ammo count to set in the magazine.
 */
void ReserveData::setAmmo(HANDLE hProcess, int32_t ammo)
{
	uintptr_t targetAddress = baseAddress + offsets::ammo;
	PatchEx((BYTE*)targetAddress, (BYTE*)&ammo, sizeof(ammo), hProcess);

}