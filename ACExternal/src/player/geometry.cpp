#include "geometry.h"

#define MATH_PI 3.14159265359
#define HALF_CIRCLE 180



float GetDistance(Vector3 localPosition, Vector3 enemyPosition)
{
	return static_cast<float>((sqrt(
		((localPosition.x - enemyPosition.x) * (localPosition.x - enemyPosition.x)) +
		((localPosition.y - enemyPosition.y) * (localPosition.y - enemyPosition.y)) +
		((localPosition.z - enemyPosition.z) * (localPosition.z - enemyPosition.z))
	)));
}

static float GetDistance2D(Vector3& localPosition, Vector3& enemyPosition)
{
	return { (float)(sqrt(
	((enemyPosition.x - localPosition.x) * (enemyPosition.x - localPosition.x))
	+ ((enemyPosition.z - localPosition.z) * (enemyPosition.z - localPosition.z))
	)) };
}

Vector3 CalculateAngle(Vector3& localPosition, Vector3& enemyPosition)
{
	float aTriangle = enemyPosition.x - localPosition.x;
	float bTriangle = enemyPosition.z - localPosition.z;
	float yTriangle = enemyPosition.y - localPosition.y;

	float triangleHyp = GetDistance2D(localPosition, enemyPosition);

	float yaw = -(float)(atan2(aTriangle, bTriangle) * (HALF_CIRCLE / MATH_PI) + HALF_CIRCLE);
	float pitch = (float)((atan2(yTriangle, triangleHyp)) * (HALF_CIRCLE / MATH_PI));

	return Vector3(yaw, pitch, 0);
}