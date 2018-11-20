#pragma once
#include "MathHelper.h"
#include <vector>

struct VertexData
{
	glm::vec3 vertex;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

class Cell
{

};

class Triangle : public Cell
{
	VertexData v1;
	VertexData v2;
	VertexData v3;
};

// Actual container for polygonal data
class PolyData
{
public:
	GLuint GetNumOfPoints() { return vertexData.size() * 3; }

protected:
	std::vector<Cell> vertexData;
};