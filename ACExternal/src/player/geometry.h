#pragma once

#include "../stdafx.h"

struct Vector3
{
	float x; //0x0000
	float y; //0x0004
	float z; //0x0008

	Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f)
		: x(x), y(y), z(z) {};

	const Vector3& operator-(const Vector3& other) noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

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
bool WorldToScreen(Vector3& position, Vector3& screen, float viewMatrix[16], int windowWidth, int windowHeight);