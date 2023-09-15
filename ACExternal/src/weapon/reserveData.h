#pragma once
#include "..\stdafx.h"


struct ReserveData
{

	ReserveData(uintptr_t baseAddress)
		: baseAddress(baseAddress) {};

	~ReserveData() {
		std::cout << "Reserve data deleted!\n";
	};

	int32_t reserveAmmo; //0x0000
	char pad_0004[36]; //0x0004
	int32_t ammo; //0x0028
	char pad_002C[36]; //0x002C
	int32_t cooldown; //0x0050
	char pad_0054[36]; //0x0054
	int32_t totalShotsFired; //0x0078
	uintptr_t baseAddress;

	void setReserveAmmo(HANDLE hProcess, int32_t reserveAmmo);
	void setAmmo(HANDLE hProcess, int32_t ammo);


};