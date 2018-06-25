#pragma once
#include "Engine\Polygon.h"

class Plane : public Poly
{
public:
	Plane()
	{
		// Defines a 1x1x1 plane
		vertexCount = 6;
		vertexData = new VertexData[vertexCount];
		vertexData[0].pos = glm::vec3(-0.5f, 0.0f, -0.5f);
		vertexData[1].pos = glm::vec3(-0.5f, 0.0f, 0.5f);
		vertexData[2].pos = glm::vec3(0.5f, 0.0f, -0.5f);

		vertexData[3].pos = glm::vec3(-0.5f, 0.0f, 0.5f);
		vertexData[4].pos = glm::vec3(0.5f, 0.0f, 0.5f);
		vertexData[5].pos = glm::vec3(0.5f, 0.0f, -0.5f);
		vertexData[0].normal = vertexData[1].normal =
			vertexData[2].normal = vertexData[3].normal =
			vertexData[4].normal = vertexData[5].normal = glm::vec3(0.0f, 1.0f, 0.0f);
		setVertexBuffer(vertexData, vertexCount);
	}
};