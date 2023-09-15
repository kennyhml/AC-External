#pragma once

#include "../stdafx.h"

struct Vector3
{
	float z; //0x0000
	float x; //0x0004
	float y; //0x0008
};


struct IntVector3
{
	int32_t z; //0x0000
	int32_t x; //0x0004
	int32_t y; //0x0008
};


struct Collider
{
	float height; //0x0000
	float crouch; //0x0004
	float stand; //0x0008
	float width; //0x000C
};
