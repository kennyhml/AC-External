#pragma once

#include "..\player.h"
#include "..\stdafx.h"
#include "weaponData.h"
#include "reserveData.h"


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
