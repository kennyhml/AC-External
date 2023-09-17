#include "geometry.h"

#define MATH_PI 3.14159265359
#define HALF_CIRCLE 180

static float GetDistance2D(Vector3& localPosition, Vector3& enemyPosition)
{
	return { (float)(sqrt(
	((enemyPosition.x - localPosition.x) * (enemyPosition.x - localPosition.x))
	+ ((enemyPosition.z - localPosition.z) * (enemyPosition.z - localPosition.z))
	)) };
}

Vector3 CalculateAngles(Vector3& localPosition, Vector3& enemyPosition)
{

	Vector3 values;

	float aTriangle = enemyPosition.x - localPosition.x;
	float bTriangle = enemyPosition.z - localPosition.z;
	float yTriangle = enemyPosition.y - localPosition.y;

	float triangleHyp = GetDistance2D(localPosition, enemyPosition);

	float yaw = -(float)(atan2(aTriangle, bTriangle) * (HALF_CIRCLE / MATH_PI) + HALF_CIRCLE);
	float pitch = (float)((atan2(yTriangle, triangleHyp)) * (HALF_CIRCLE / MATH_PI));

	values.x = yaw;
	values.y = pitch;
	values.z = 0;
	return values;
}