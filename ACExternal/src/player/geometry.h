#pragma once

#include "../stdafx.h"

struct Vector3
{
	Vector3(
		const float x = 0.f,
		const float z = 0.f,
		const float y = 0.f)
		: x(x), z(z), y(y) {};

	float x; //0x0000
	float z; //0x0004
	float y; //0x0008

	const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}
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

Vector3 CalculateAngle(Vector3& localPosition, Vector3& enemyPosition);
float GetDistance(Vector3 localPosition, Vector3 enemyPosition);