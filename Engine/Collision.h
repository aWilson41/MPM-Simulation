#pragma once

#include "MathHelper.h"

class Collision
{
public:
	static void IntersectTriangleTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 e, glm::vec3 f)
	{
		glm::vec3 line1 = a - b;
		glm::vec3 line2 = b - c;
		glm::vec3 line3 = c - a;
	}

	static void IntersectLineTriangle()
	{

	}
};