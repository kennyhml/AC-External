#include "geometry.h"

#define PI 3.14159265359f
#define HALF_CIRCLE 180

float GetDistance(Vector3 localPosition, Vector3 enemyPosition)
{
	return static_cast<float>((sqrt(
		((localPosition.x - enemyPosition.x) * (localPosition.x - enemyPosition.x)) +
		((localPosition.y - enemyPosition.y) * (localPosition.y - enemyPosition.y)) +
		((localPosition.z - enemyPosition.z) * (localPosition.z - enemyPosition.z))
	)));
}

Vector3 CalculateAngle(Vector3& localPosition, Vector3& enemyPosition)
{
	float xDiff = enemyPosition.x - localPosition.x;
	float yDiff = enemyPosition.y - localPosition.y;
	float zDiff = (enemyPosition.z + 0.25f) - localPosition.z;

	float distance = GetDistance(localPosition, enemyPosition);

	float yaw = -atan2(xDiff, yDiff) / PI * 180.f + 180.f;
	float pitch = asinf(zDiff / distance) * 180.f / PI;

	return Vector3(yaw, pitch, 0);
}

bool WorldToScreen(Vector3& position, Vector3& screen, float viewMatrix[16], int windowWidth, int windowHeight)
{
	float x = position.x * viewMatrix[0] + position.y * viewMatrix[4] + position.z * viewMatrix[8] + viewMatrix[12];
	float y = position.x * viewMatrix[1] + position.y * viewMatrix[5] + position.z * viewMatrix[9] + viewMatrix[13];
	float z = position.x * viewMatrix[2] + position.y * viewMatrix[6] + position.z * viewMatrix[10] + viewMatrix[14];
	float w = position.x * viewMatrix[3] + position.y * viewMatrix[7] + position.z * viewMatrix[11] + viewMatrix[15];

	if (w < 0.1f) { return false; }

	Vector3 result = { x / w, y / w, z / w };

	screen.x = (windowWidth / 2 * result.x) + (result.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * result.y) + (result.y + windowHeight / 2);
	return true;
}
