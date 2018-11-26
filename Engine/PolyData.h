#pragma once
#include "MathHelper.h"
#include <vector>

enum CellType
{
	POINT,
	LINE,
	TRIANGLE,
	QUAD
};

struct VertexData
{
	VertexData()
	{
		pos = glm::vec3(0.0f);
		normal = glm::vec3(0.0f);
		texCoords = glm::vec2(0.0f);
	}
	VertexData(glm::vec3 vertex, glm::vec3 normal, glm::vec2 texCoords)
	{
		VertexData::pos = vertex;
		VertexData::normal = normal;
		VertexData::texCoords = texCoords;
	}

	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Cell
{
public:
	VertexData v1;
};
class Line : public Cell
{
public:
	VertexData v2;
};
class Triangle : public Line
{
public:
	VertexData v3;
};
class Quad : public Triangle
{
public:
	VertexData v4;
};

// Actual container for polygonal data. Can stores points, lines, triangles, and quads
class PolyData
{
public:
	~PolyData() { delete[] data; }

public:
	UINT getNumOfPoints() { return numPts; }
	UINT getNumOfCells() { return numCells; }
	CellType getCellType() { return type; }

	void* getData() { return data; }
	Cell* getPointData() { return data; }
	Line* getLineData() { return static_cast<Line*>(data); }
	Triangle* getTriangleData() { return static_cast<Triangle*>(data); }
	Quad* getQuadData() { return static_cast<Quad*>(data); }
	void allocate(UINT cellCount, CellType type)
	{
		if (data != nullptr)
			delete[] data;

		PolyData::type = type;
		PolyData::numCells = cellCount;
		if (type == POINT)
		{
			data = new Cell[cellCount];
			memset(data, 0, sizeof(Cell) * cellCount);
			numPts = cellCount;
		}
		else if (type == LINE)
		{
			data = new Line[numCells];
			memset(data, 0, sizeof(Line) * cellCount);
			numPts = cellCount * 2;
		}
		else if (type == TRIANGLE)
		{
			data = new Triangle[numCells];
			memset(data, 0, sizeof(Triangle) * cellCount);
			numPts = cellCount * 3;
		}
		else if (type == QUAD)
		{
			data = new Quad[cellCount];
			memset(data, 0, sizeof(Quad) * cellCount);
			numPts = cellCount * 4;
		}
	}

protected:
	Cell* data = nullptr;
	UINT numPts = -1;
	UINT numCells = -1;
	CellType type = TRIANGLE;
};