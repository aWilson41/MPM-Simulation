#include "Camera.h"

void Camera::reset()
{
	fov = 45.0f;
	eyePos = glm::vec3(0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::mat4(1.0f);

	// 1, 0, 0, 0
	// 0, 1, 0, 0
	// 0, 0, 1, 0
	// 0, 0, 1, 0 default proj matrix. Causes a z divide
	proj = glm::mat4(1.0f);
	proj[2][3] = 1.0f;
	proj[3][3] = 0.0f;
}