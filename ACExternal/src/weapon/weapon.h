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
		delete data;
		data = nullptr;
		delete reserveData;
		reserveData = nullptr;
		std::cout << "Weapon deleted!\n";
	}

	uint32_t vTable; //0x0000
	int32_t ID; //0x0004
	int32_t owner; //0x0008
	WeaponData* data; //0x000C
	ReserveData* reserveData; //0x0010
	uintptr_t baseAddress;
};

Weapon LoadWeapon(HANDLE hProcess, uintptr_t weaponAddress);
