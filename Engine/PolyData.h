#pragma once
#include "MathHelper.h"
#include <vector>

struct VertexData
{
	VertexData()
	{
		pos = glm::vec3(0.0f);
		normal = glm::vec3(0.0f);
		//texCoords = glm::vec2(0.0f);
	}
	VertexData(glm::vec3 vertex, glm::vec3 normal)//, glm::vec2 texCoords)
	{
		VertexData::pos = vertex;
		VertexData::normal = normal;
		//VertexData::texCoords = texCoords;
	}

	glm::vec3 pos;
	glm::vec3 normal;
	//glm::vec2 texCoords;
};

class Triangle
{
public:
	VertexData v1;
	VertexData v2;
	VertexData v3;
};

// Actual container for polygonal data
class PolyData
{
public:
	UINT getNumOfPoints() { return static_cast<UINT>(cellData.size() * 3); }
	void* getData() { return cellData.data(); }
	void allocate(UINT numCells)
	{
		cellData.clear();
		cellData.resize(numCells);
		memset(cellData.data(), 0, sizeof(Triangle) * numCells);
	}

protected:
	std::vector<Triangle> cellData;
};