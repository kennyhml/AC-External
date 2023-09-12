#pragma once

#include "geometry.h"
#include <string>
#include "stdafx.h"

enum PlayerState { ALIVE, DEAD, SPECTATOR };


struct Player
{
	Player(uintptr_t baseAddress, HANDLE hProcess)
		: baseAddress(baseAddress) {
	
		getHealth(hProcess);
		getArmor(hProcess);
		getTeam(hProcess);
		getState(hProcess);
		getName(hProcess);
		getPosition(hProcess);
	};

	uintptr_t baseAddress;

	int health;
	int armor;
	std::string name;
	PlayerState state;

	int distance = 0;
	int team;
	bool isEnemy;

	vec2 angle;
	vec3 bodyPosition;
	vec3 headPosition;
	vec3 angleTo;

	void getHealth(HANDLE hProcess);
	void getArmor(HANDLE hProcess);

	void getDistance(Player localPlayer);
	void getAngleTo(Player localPlayer);
	void getTeam(HANDLE hProcess);

	void getState(HANDLE hProcess);
	void getName(HANDLE hProcess);
	void getPosition(HANDLE hProcess);
	void getAngle(HANDLE hProcess);

	bool isValid();
	bool isVisible();


};
